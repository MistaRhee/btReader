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
    std::string logLoc = currentDateTime() + " btReader.log";
    mLog = new(__logger::cLogger, logLoc);
    if(checkDependencies()){
        preComp();
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string e = currentDateTime() + ": [btReader.cpp] - SDL could not initialize! SDL_Error:";
            e += SDL_GetError();
            fprintf(stderr, "%s \n", e.c_str());
            exit(123);
        }
        mWindow = SDL_CreateWindow(
                "btReader - By MistaRhee and NoOne2246", 
                SDL_WINDOWPOS_CENTERED, 
                SDL_WINDOWPOS_CENTERED, 
                1024, 600, 
                SDL_WINDOW_SHOWN
                );
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
        currThreads = 1;
        getUserProfile();
    }
    startRunTime = SDL_GetTicks();
    whereAt = list;
    running = 1;
}

cMain::~cMain(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void cMain::preComp(){
    if(!fileExists("data/novels.db")){
        printf("%s: [btReader.cpp] - No database found! Rebuilding novel list from site! \n", currentDateTime().c_str());
        createDatabase();
    }
    else if(!readDatabase()){
        printf("%s: [btReader.cpp] - Error! The database is corrupt and cannot be read. Overwriting the database! \n", currentDateTime().c_str());
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
                        std::string mError = currentDateTime() + ": ";
                        mError += "[btReader.cpp] - Check Dependencies Error: Essential Folder doesn't exist (";
                        mError += it->attribute("sauce").value();
                        mError += ")";
                        setError(mError);
                        printf("%s \n", mError.c_str());
                        rVal = 0;
                    }
                    else createFolder(it->attribute("sauce").value());
                }
            }
            else if(type.compare("file") == 0){
                /* For important system files */
                if(!fileExists(it->attribute("sauce").value())){
                    std::string mError = currentDateTime() + ": ";
                    mError += "[btReader.cpp] - Check Dependencies Error: Essential File doesn't exist: ";
                    mError += it->attribute("sauce").value();
                    setError(mError);
                    printf("%s \n", mError.c_str());
                    rVal = 0;
                }
            }
            else{
                /* Invalid manifest type... I mean, if it isn't a file nor folder, what is it???? */
                std::string mError = currentDateTime() + ": ";
                mError += "[btReader.cpp] - Check Dependencies Error: Invalid typing: ";
                mError += type;
                setError(mError);
                printf("%s \n", mError.c_str());
                rVal = 0;
            }
        }
    }
    else{
        std::string e = currentDateTime() + " [btReader.cpp] Load config error! ";
        e += "manifest.db could not be parsed. Error: ";
        e += mRes.description();
        setError(e);
        fprintf(stderr, "%s \n", e.c_str());
    }

    return rVal;
}

void cMain::getUserProfile(){
    /* All the settings/GUI configs etc. shall now be found here */
    if(!fileExists("system/user.profile")){
        /* No existing profile exists create new one using default settings */
        printf("%s: [btReader.cpp] - Critical Error! User Profile does not exist! \n", currentDateTime().c_str());
        exit(-1);
    }
    else{
        try{
            pugi::xml_document doc;
            pugi::xml_parse_result res = doc.load_file("system/user.profile");
            pugi::xml_node rootNode = doc.child("profile");
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
        catch(mException& e){
            std::string mError = currentDateTime() + ": ";
            mError += "[btReader.cpp] - getUserProfile Error: ";
            mError += e.what();
            setError(mError);
            printf("%s \n", mError.c_str());;
        }
    }
}

void cMain::setError(std::string mError){
    error += mError+"\n";
}

std::string cMain::getError(){
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error.c_str(), NULL);
    return error;
}

bool cMain::run(){
    bool rVal = 1;
    bool running = 1;
    int startTick = SDL_GetTicks();
    while(running){
        if(error.size() > 0){
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
    printf("Runtime = %d", SDL_GetTicks() - startRunTime);
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
        std::string mError = currentDateTime() + ": ";
        mError += "[btReader.cpp] - Error: In Render, ";
        mError += e.what();
        setError(mError);
        printf("%s \n", mError.c_str());
    }
    /* Display the image \0/ */
    SDL_RenderPresent(mRenderer);
    /* Come again to render more shit :D */
}

void cMain::commitSudoku(){
    this->running = 0; //Wai you do dis?
}
