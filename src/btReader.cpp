#include "btReader.hpp"

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
    else return true;
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

cMain::cMain(){
    std::string logLoc = "logs/";
    logLoc = currentDateTime() + " btReader.log";
    mLog = new __logger::cLogger(logLoc);
    logLoc.clear();
    mLog->start().detach();
    mLog->log("[btReader.cpp] - Started logging.");
    if(checkDependencies()){
        preComp();
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string e = "[btReader.cpp] Error: SDL could not initialize! SDL_Error:";
            e += SDL_GetError();
            mLog->log(e);
            mLog->kill();
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
        getUserProfile();
        this->startRunTime = SDL_GetTicks();
        this->whereAt = list;
        this->running = 1;
    }
    else this->running = 0;
}

cMain::~cMain(){
    this->mLog->kill();
    delete(this->mLog);
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void cMain::preComp(){
    if(!fileExists("data/novels.db")){
        /* Remove any old data that might still be there */
        remove("data/novels");
        remove("data/images");
        remove("data/temp");

        /* Recreate the non-essential folders */
        checkDependencies();
        mLog->log("[btReader.cpp] Warning: No database found! Rebuilding novel list from site!");
        createDatabase();
    }
    else if(!readDatabase()){
        /* Remove any old data that might still be there */
        remove("data/novels");
        remove("data/images");
        remove("data/temp");
        remove("data/novels.db");

        /* Recreate the non-essential folders */
        checkDependencies();
        mLog->log("[btReader.cpp] Error: The database is corrupt and cannot be read. Overwriting the database!");
        createDatabase();
    }
    /* Setting default colours! (will be overwritten by the XML file if it
     * exists
     */
    /* Clearing the maps, just in case data is leaked (shouldn't though) */
    fonts.clear();
    colours.clear();
    mContents.clear();
    replaceDatabase();

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
    beatOff::cNovelList* mList = (beatOff::cNovelList*)mContents[list];
    for(auto i = novelDB.begin(); i != novelDB.end(); ++i){
        mList->addNovel(
                i->first, 
                atoi(config["novelList"]["font"].c_str()),
                fonts["novelList"]
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
        mLog->kill();
        exit(-1);
    }
    else{
        try{
            pugi::xml_document doc;
            pugi::xml_parse_result res = doc.load_file("system/user.profile");
            pugi::xml_node rootNode = doc.child("profile");
            if(res){
                /* Load XML File into the map */
                for(auto it = rootNode.begin(); it != rootNode.end(); ++it){
                    std::string name = it->name();
                    for(auto ot = it->begin(); ot != it->end(); ++ot){
                        config[name][ot->attribute("key").value()] = ot->attribute("value").value();
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
                    for(auto it = config["keyBindings"].begin(); it != config["keyBindings"].end(); ++it){
                        this->mKeys.addMapping(atoi(it->first.c_str()), it->second);
                    }

                    /* Remove that entry from the map to save memory */
                    config.erase("keyBindings");
                }
            }
            else{
                std::string e = "[btReader.cpp] Error: ";
                e += "user.profile could not be parsed. Description: ";
                e += res.description();
                throw(e);
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
    while(running){
        rVal = 1;
        if(error){
            rVal = 0;
            running = 0;
        }
        else{
            startTick = SDL_GetTicks();
            while(SDL_GetTicks() < startTick+FPS_CAP){
                processEvents();
                update();
            }
            render();
        }
    }
    replaceDatabase();
    std::string runtime = "Runtime = ";
    runtime += std::to_string(SDL_GetTicks() - startRunTime);
    mLog->log(runtime);
    return rVal;
}

void cMain::update(){
    /* ~~La da da~~ */
    return;
}

void cMain::render(){
    /* Clear the screen with the background colour */
    auto found = colours.find("clear");
    SDL_SetRenderDrawColor(mRenderer, found->second.r, found->second.g, found->second.b, found->second.a);
    SDL_RenderClear(mRenderer);
    try{
        /* Draw the content first */
        mContents[whereAt]->render();
        /* Draw the "interface" over the content. */
        mContents[menu]->render();
    }
    catch(mException& e){
        /* Render errors should be logged in the respective rendering file */
        setError();
    }
    /* Display the image \0/ */
    SDL_RenderPresent(mRenderer);
    /* Come again to render more shit :D */
}

void cMain::commitSudoku(){
    this->running = 0; //Wai you do dis?
}
