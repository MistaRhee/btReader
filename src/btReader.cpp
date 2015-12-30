#include "btReader.hpp"

//Font list lookup macro
#define FONT_LOOKUP(...) this->config["fontList"].find(__VA_ARGS__)->second["sauce"]

#ifdef _WIN32
#include <windows.h>
#endif
#if __unix__
#include <dirent.h>
#endif

#ifdef _WIN32
inline bool dirExists(const std::string& dirName) {
    DWORD ftyp = GetFileAttributesA(dirName.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif
#ifdef __unix__
inline bool dirExists(const std::string& dirName){
    DIR* myDir = NULL;
    myDir = opendir(dirName.c_str());
    if(myDir == NULL) return false;
    else{
        closedir(myDir);
        return true;
    }
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif

inline bool fileExists (const std::string& name){
    if(FILE *file = fopen(name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
        return false;
    }
}

inline int fastMax(int x, int y) { return (((y-x)>>(31))&(x^y))^y; }

cMain::cMain(){
    std::string logLoc = "logs/";
    logLoc += "btReader.log";
    mLog = new __logger::cLogger(logLoc);

#ifndef __LOGGER_NOTHREAD__
    mLog->start().detach();
#endif

    mLog->log("[btReader.cpp] - Started logging.");
    logLoc.clear();
    this->error = 0; //Program can break during preComp and Check dependencies
    if(checkDependencies()){
        preComp();
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string e = "[btReader.cpp] Error: SDL could not initialize! SDL_Error:";
            e += SDL_GetError();
            mLog->log(e);
#ifndef __LOGGER_NOTHREAD__
            mLog->kill();
#endif
            exit(123);
        }
        this->mWindow = SDL_CreateWindow(
                "btReader - By MistaRhee and NoOne2246", 
                SDL_WINDOWPOS_CENTERED, 
                SDL_WINDOWPOS_CENTERED, 
                1024, 600, 
                SDL_WINDOW_SHOWN
                );
        this->mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
        this->currThreads = 1;
        this->startRunTime = SDL_GetTicks();
        this->whereAt = list;
        this->updatedDB = 0;
        this->running = 1;
    }
    else this->running = 0;
}

cMain::~cMain(){
    close();
}

void cMain::close(){
    this->mLog->log("[btReader.cpp] Info: Shutting down logging system! Goodbye, cruel world");

#ifndef __LOGGER_NOTHREAD__
    this->mLog->kill();
    /* Block until the logger has finished */
    this->mLog->done.lock();
    this->mLog->done.unlock();
#endif

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void cMain::preComp(){
    if(!fileExists("data/novels.db")){
        /* Remove any old data that might still be there */
        remove("data/");

        /* Recreate the non-essential folders */
        checkDependencies();
        mLog->log("[btReader.cpp] Warning: No database found! Rebuilding novel list from site!");
        createDatabase();
    }
    else if(!readDatabase()){
        /* Remove any old data that might still be there */
        remove("data/");

        /* Recreate the non-essential folders */
        checkDependencies();
        mLog->log("[btReader.cpp] Error: The database is corrupt and cannot be read. Overwriting the database!");
        createDatabase();
    }
    replaceDatabase();

    /* Clearing the maps, just in case data is leaked (shouldn't though) */
    this->colours.clear();
    this->mContents.clear();

    /* Add the content objects */
    this->mContents[menu] = new beatOff::cMenu();
    this->mContents[list] = new beatOff::cNovelList(this->mLog);
    this->mContents[details] = new beatOff::cNovelDetails();

    /* Grab user profile */
    getUserProfile();

    SDL_Rect mRect;
    mRect.x = std::stoi(this->config["interface"].find("content")->second["x"]);
    mRect.y = std::stoi(this->config["interface"].find("content")->second["y"]);
    mRect.h = std::stoi(this->config["interface"].find("content")->second["h"]);
    mRect.w = std::stoi(this->config["interface"].find("content")->second["w"]);

    ((beatOff::cNovelList*)this->mContents[list])->setRect(mRect);
    ((beatOff::cNovelDetails*)this->mContents[details])->setRect(mRect);

    /* TODO: Uncomment when appropriate contents are implemented */
    //((beatOff::cSettings*)this->mContents[settings])->setRect(mRect);
    //((beatOff::cDlList*)this->mContents[dlList])->setRect(mRect);

    /* Uncomment when appropriate object is completed */
    //this->mContents[settings] = new beatOff::cSettings();
    //this->mContents[dlList] = new beatOff::dlList();

    /* Creating a colour map */
    SDL_Colour temp;
    temp.r = 255;
    temp.g = 255;
    temp.b = 255;
    temp.a = 255;
    colours.insert(std::make_pair("back", temp));
    temp.r = 0;
    temp.g = 0;
    temp.b = 0;
    colours.insert(std::make_pair("clear", temp));
    colours.insert(std::make_pair("text", temp));

    /* Start the update process in the background */
    std::thread(&cMain::updateDatabase, this).detach();
    this->updating = 1;

    /* Build a novelList based off what we currently have */
    beatOff::cNovelList* mList = (beatOff::cNovelList*)mContents[list];
    for(auto i = novelDB.begin(); i != novelDB.end(); ++i){
        mList->addNovel(
                i->first, 
                std::stoi(config["novelList"].find("size")->second.find("value")->second),
                FONT_LOOKUP(this->config["novelList"].find("font")->second["value"])
                );
    }
}

bool cMain::checkDependencies(){ //Checking if directories exist and important files are there.
    bool rVal = 1;
    pugi::xml_document mDoc;
    pugi::xml_parse_result mRes = mDoc.load_file("manifest.db");

    if(mRes){
        pugi::xml_node rootNode = mDoc.child("content");
        for(auto it = rootNode.begin(); it != rootNode.end(); ++it){ // Since it's only a single root and a bunch of nodes (one level deep)
            std::string type = it->name();
            if(type.compare("folder") == 0){
                /* For program required folders */
                if(!dirExists(it->attribute("sauce").value())){
                    std::string essential = it->attribute("essential").value();
                    if(!essential.compare("true")){
                        /* Important shit that can't just be made on the spot */
                        std::string mError = "[btReader.cpp] Error: Essential Folder doesn't exist (";
                        mError += it->attribute("sauce").value();
                        mError += ")";
                        mLog->log(mError);
                        rVal = 0;
                    }
                    else createFolder(it->attribute("sauce").value());
                }
            }
            else if(type.compare("file") == 0){
                /* For important system files */
                if(!fileExists(it->attribute("sauce").value())){
                    std::string mError = "[btReader.cpp] Error: Essential File doesn't exist: ";
                    mError += it->attribute("sauce").value();
                    mLog->log(mError);
                    rVal = 0;
                }
            }
            else{
                /* Invalid manifest type... I mean, if it isn't a file nor folder, what is it???? */
                std::string mError = "[btReader.cpp] - Error: Invalid typing: ";
                mError += type;
                mLog->log(mError);
                rVal = 0;
            }
        }
    }
    else{
        std::string e = "[btReader.cpp] Error:";
        e += "manifest.db could not be parsed. Description: ";
        e += mRes.description();
        mLog->log(e);
    }

    return rVal;
}

void cMain::getUserProfile(){
    /* All the settings/GUI configs etc. shall now be found here */
    if(!fileExists("system/user.profile")){
        /* No existing profile exists create new one using default settings */
        mLog->log("[btReader.cpp] Critical: User Profile does not exist!");
#ifndef __LOGGER_NOTHREAD
        mLog->kill();
#endif
        exit(-1);
    }
    else{
        try{
            pugi::xml_document doc;
            pugi::xml_parse_result res = doc.load_file("system/user.profile");
            pugi::xml_node rootNode = doc.child("config");
            if(res){
                /* Load XML File into the map */
                for(auto it: rootNode.children()){
                    std::string name = it.name();
                    for(auto ot: it.children()){
                        std::map<std::string, std::string> tempAtt;
                        for(auto attribute: ot.attributes()){
                            tempAtt[attribute.name()] = attribute.value();
                        }
                        this->config[name].insert(std::make_pair(ot.name(), std::move(tempAtt)));
                    }
                }

                /* Get keybindings out of the map. If there aren't keybindings, resort to default */
                if(!config.count("keyBindings")){
                    /* Giff default plz! -> Not sure if this is legit.... */
                    this->mKeys.addMapping(SDLK_UP, "up");
                    this->mKeys.addMapping(SDLK_DOWN, "down");
                    this->mKeys.addMapping(SDLK_LEFT, "left");
                    this->mKeys.addMapping(SDLK_RIGHT, "right");
                }
                else{
                    /* Extract keys */
                    std::string id;
                    for(auto it = this->config["keybindings"].begin(); it != this->config["keybindings"].end(); ++it){
                        /* Do a sanity check -> First should be key */
                        if(it->first != "key"){
                            /* Log it! Don't set exit */
                            this->mLog->log(std::string("[btReader.cpp] Error: Keybindings has an non-\"key\" entry! Entry type: ")+it->first);
                        }
                        else{
                            this->mKeys.addMapping(std::stoi(it->second.find("code")->second), it->second.find("id")->second);
                        }
                    }

                    /* Remove that entry from the map to save memory */
                    config.erase("keyBindings");
                }

                if(!config.count("menu")){
                    /* Something bad just happened, the menu doesn't exist in the user.profile file */
                    this->mLog->log("[btReader.cpp] Fatal: User.profile doesn't contain an entry for menu! ");
                    setError();
                }
                else{
                    beatOff::cMenu* mMenu = (beatOff::cMenu*)this->mContents[menu];
                    for(auto it = this->config["menu"].begin(); it != this->config["menu"].end(); ++it){
                        if(it->first == "image"){
                            /* Selectively add images */
                            if(it->second["name"] == "settings" or it->second["name"] == "downloads"){
                                mMenu->addImage(
                                        it->second["name"],
                                        it->second["sauce"],
                                        std::stoi(it->second["x"]),
                                        std::stoi(it->second["y"]),
                                        std::stoi(it->second["h"]),
                                        std::stoi(it->second["w"])
                                        );
                            }
                        }
                        else if (it->first == "button"){
                            mMenu->addButton(
                                    it->second["name"],
                                    it->second["text"],
                                    FONT_LOOKUP(it->second["font"]),
                                    std::stoi(it->second["size"]),
                                    std::stoi(it->second["x"]),
                                    std::stoi(it->second["y"]),
                                    std::stoi(it->second["h"]),
                                    std::stoi(it->second["w"])
                                    );
                        }
                        else if (it->first == "font"){
                            /* FONT SHOuLD APPEAR BEFORE EVERYTHING */
                            mMenu->setFont(FONT_LOOKUP(it->second["name"]));
                        }
                        else{
                            /* Invalid config name, logging it */
                            this->mLog->log(std::string("[btReader.cpp] Warning: Menu config has invalid handle ") + it->first + ". Ignoring");
                        }
                    }
                    /* Select the novelList button -> HACKY, but deal with it*/
                    mMenu->selectButton("novelList");
                }
            }
            else{
                std::string e = "[btReader.cpp] Error: ";
                e += "user.profile could not be parsed. Description: ";
                e += res.description();
                throw(mException(e));
            }
        }
        catch(mException& e){
            mLog->log(e.what());
            setError();
        }
    }
}

void cMain::setError(){
    this->error = 1;
}

bool cMain::run(){
    bool rVal = 0;
    int startTick = SDL_GetTicks();
    this->mLog->log("[btReader.cpp] Info: Running");
    while(running){
        rVal = 1;
        if(error){
            rVal = 0;
            running = 0;
        }
        else{
            startTick = SDL_GetTicks();
            processEvents();
            update();
            if(SDL_GetTicks()-startTick < FPS_CAP) SDL_Delay(fastMax(0, FPS_CAP - int(SDL_GetTicks() - startTick)));
            render();
        }
    }
    mLog->log("[btReader.cpp] Info: Currently exiting from the main loop just fine. Replacing database!");
    replaceDatabase();
    mLog->log("[btReader.cpp] Info: Database successfully replaced! Now shutting down!");
    std::string runtime = "[btReader.cpp] Info: Runtime = ";
    runtime += std::to_string(SDL_GetTicks() - startRunTime);
    mLog->log(runtime);
    return rVal;
}

void cMain::render(){
    /* Clear the screen with the background colour */
    auto found = colours.find("clear");
    SDL_SetRenderDrawColor(mRenderer, found->second.r, found->second.g, found->second.b, found->second.a);
    SDL_RenderClear(mRenderer);
    try{
        /* Draw the content first */
        switch(whereAt){
            case list:
                ((beatOff::cNovelList*)mContents[whereAt])->render(mRenderer);
                break;

            case details:
                break;

            default:
                this->mLog->log(std::string("[btReader.cpp] Error: Invalid location reached for whereAt in render: ") + std::to_string(whereAt));

        }
        /* Draw the "interface" over the content. */
        mContents[menu]->render(mRenderer);
    }
    catch(mException& e){
        /* Render errors should be logged in the respective rendering file */
        this->mLog->log(e.what());
        setError();
    }
    /* Display the image \0/ */
    SDL_RenderPresent(mRenderer);
    /* Come again to render more shit :D */
}

void cMain::update(){
    /* Check menu location */
    if(this->mContents[menu]->state == go){
        beatOff::cMenu* mMenu = (beatOff::cMenu*)(this->mContents[menu]);
        places_t goingTo = mMenu->getSelected();
        switch(goingTo){
            case list:
                break;

            case dlList: //Currently no UI, it'll just update the database in the background
                if(!updating){
                this->updating = 1;
                std::thread(&cMain::updateDatabase, this).detach();
                }
                else{
                    this->mLog->log("[btReader.cpp] Warning: Already updating database! Ignoring new user request!");
                }
                break;

            case settings: //TODO after the competion of settings
                break;

            default:
                this->mLog->log("[btReader.cpp] Error: Recieved invalid go to location from mMenu->getSelected!");
                break;
        }
    }
    /* Check updates/updating status and change the image accordingly */
    if(this->updating){
        /* Check if it's finished updating */
        if(this->novelDBLock.try_lock()){
            this->novelDBLock.unlock();
            this->updating = 0;

            auto range = this->config["menu"].equal_range("image");
            for(auto it = range.first; it != range.second; ++it){
                if(it->second["name"] == "downloads") ((beatOff::cMenu*)this->mContents[menu])->changeImage("downloads", it->second["sauce"]);
            }
        }
        else{
            auto range = this->config["menu"].equal_range("image");
            for(auto it = range.first; it != range.second; ++it){
                if(it->second["name"] == "downloads-active") ((beatOff::cMenu*)this->mContents[menu])->changeImage("downloads", it->second["sauce"]);
            }
        }
    }

    /* Check each individual location */
    switch(whereAt){
        case list:
            /* Check if the DB has been updated, if it has, update the novelList accordingly */
            if(this->updatedDB){
                this->updatedDB = 0;
            }
            break;

        case details://TODO on completion of novelDetails
            break;

        case settings: //TODO on completion of settigns
            break;

        case dlList: //TODO on completion of DL List
            break;

        default:
            this->mLog->log("[btReader.cpp] Error! Invalid whereAt location");
            setError();
            break;
    }
}

void cMain::commitSudoku(){
    this->running = 0; //Wai you do dis?
}
