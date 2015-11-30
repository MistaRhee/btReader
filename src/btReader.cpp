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
        getObjects();
        getUserProfile();
    }
    startRunTime = SDL_GetTicks();
    whereAt = list;
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
    getObjects();
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
    XMLNode mainNode = XMLNode::openFileHelper("manifest.db", "content");
    for(int i = 0, j = mainNode.nChildNode(); i < j; i++){
        XMLNode currNode = mainNode.getChildNode(i);
        std::string type = currNode.getName();
        if(type.compare("folder") == 0){
            /* For system folders */
            std::string essential = currNode.getAttribute("essential");
            if(!dirExists(currNode.getAttribute("sauce"))){
                if(!essential.compare("true")){
                    /* Important shit that can't just be made on the spot */
                    std::string mError = currentDateTime() + ": ";
                    mError += "[btReader.cpp] - Check Dependencies Error: Essential Folder doesn't exist (";
                    mError += currNode.getAttribute("sauce");
                    mError += ")";
                    setError(mError);
                    printf("%s \n", mError.c_str());
                    rVal = 0;
                }
                else{
                    createFolder(currNode.getAttribute("sauce"));
                }
            }
        }
        else if(type.compare("file") == 0){
            /* For important system files */
            if(!fileExists(currNode.getAttribute("sauce"))){
                std::string mError = currentDateTime() + ": ";
                mError += "[btReader.cpp] - Check Dependencies Error: Essential File doesn't exist: ";
                mError += currNode.getAttribute("sauce");
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
            XMLNode mainNode = XMLNode::openFileHelper("system/user.profile", "profile");
            XMLNode currChild;
            /* Load XML File into the map */
            for(int i = 0, j = mainNode.nChildNode(); i < j; i++){
                currChild = mainNode.getChildNode(i);
                std::string name = currChild.getName();
                for(int k = 0, l = currChild.nChildNode(); k < l; k++){
                    config[name][currChild.getChildNode(k).getAttribute("key")] = currChild.getChildNode(k).getAttribute("value");
                }
            }

            /* Get keybindings out of the map. If there aren't keybindings, resort to default */
            if(!config.count("keyBindings")){
                /* Giff default plz! -> Not sure if this is legit.... */
                mKeys.addMapping(SDLK_UP, "up");
                mKeys.addMapping(SDLK_DOWN, "down");
                mKeys.addMapping(SDLK_LEFT, "left");
                mKeys.addMapping(SDLK_RIGHT, "right");
            }
            else{
                /* Extract keys */

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
//            while(SDL_GetTicks() < startTick+FPS_CAP){
                processEvents();
//                update();
//            }
            render();
        }
    }
    replaceDatabase();
    printf("Runtime = %d", SDL_GetTicks() - startRunTime);
    return rVal;
}

void cMain::update(){
    /* ~~La da da~~ */
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
    running = 0; //Wai you do dis?
}
