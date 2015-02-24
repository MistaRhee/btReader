#include "btreader.hpp"

#if defined(WIN32)
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
#elif defined(UNIX)
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
            printf("SDL could not initialize! SDL_Error: %s \n", SDL_GetError());
        }
        mWindow = SDL_CreateWindow("btReader - By MistaRhee and NoOne2246", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_SHOWN);
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
        currThreads = 1;
        getObjects();
    }
}

cMain::~cMain(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void cMain::preComp(){
    if(!fileExists("data/novels.db")){
        printf("No database found! Rebuilding novel list from site! \n");
        createDatabase();
    }
    else if(!readDatabase()){
        printf("Error! The database is corrupt and cannot be read. Overwriting the database! \n");
        createDatabase();
    }
    /* Setting default colours! (will be overwritten by the XML file if it
     * exists
     */
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
    for(auto i = novelDB.begin(); i != novelDB.end(); ++i){
        mNovelList->addNovel(i->first);
    }
}

bool cMain::checkDependencies(){ //Checking if directories exist and important files are there.
    XMLNode mainNode = XMLNode::openFileHelper("manifest.db", "content");
    for(int i = 0, j = mainNode.nChildNode(); i < j; i++){
        XMLNode currNode = mainNode.getChildNode(i);
        std::string type = currNode.getName();
        if(type.compare("folder") == 0){
            /* For system folders */
            std::string essential = currNode.getAttribute("essential");
            if(!dirExist(currNode.getAttribute("sauce"))){
                if(essential.compare("true")){
                    /* Important shit that can't just be made on the spot */
                    printf("Essential Folder doesn't exist \n");
                    std::string mError = "Check Dependencies Error - Essential Folder doesn't exist: " + currNode.getAttribute("sauce");
                    setError(mError);
                }
                else{
                    createFolder(currNode.getAttribute("sauce"));
                }
            }
        }
        else if(type.compare("file") == 0){
            /* For important system files */
            if(!fileExists(currNode.getAttribute("sauce"))){
                printf("Essential File doesn't exist \n");
                std::string mError = "Check Dependencies Error - Essential File doesn't exist: " + currNode.getAttribute("sauce");
                setError(mError);
            }
        }
        else{
            /* Invalid manifest type... I mean, if it isn't a file nor folder, what is it???? */
            printf("Invalid typing. \n");
            std::string mError = "Check Dependencies Error - Invalid typing: " + type;
            setError(mError);
        }

    }
}

void cMain::setError(std::string mError){
    error += mError+"\n";
}

std::string cMain::getError(){
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error - Program hasn't exited correctly", error.c_str(), NULL);
    return error;
}

void cMain::getObjects(){
    XMLNode mainNode = XMLNode::openFileHelper("system/menu.xml", "menu");
    for(int i = 0, j = mainNode.nChildNode(); i < j; i++){
        XMLNode curr = mainNode.getChildNode(i);
        std::string name = curr.getName;
        std::string id;
        if(name.compare("font") == 0){
            id = curr.getAttribute("name");
            std::string sauce = curr.getAttribute("sauce");
            fonts.insert(std::make_pair(id, sauce));
        }
        else if(name.compare("image") == 0){
            id = curr.getAttribute("name");
            beatOff::cImage newImage;
            newImage.setPicLoc(curr.getAttribute("sauce"));
            newImage.setPos(atoi(curr.getAttribute(x)), atoi(curr.getAttribute(y)));
            newImage.setSize(atoi(curr.getAttribute(h)), atoi(curr.getAttribute(w)));
            images.insert(std::make_pair(id, std::move(newImage));
        }
        else if(name.compare("button") == 0){
            id = curr.getAttribute("name");
            beatOff::cButton newButton;
            auto textCol = colours.find("text");
            auto boxCol = colours.find("back");
            newButton.setText(curr.getAttribute("text"));
            newButton.setTextSize(curr.getAttribute("size"));
            newButton.setTextCol(textCol->r, textCol->g, textCol->b, textCol->a);
            newButton.setBoxCol(boxCol->r, boxCol->g, boxCol->g, boxCol->a);
            newButton.setFont(curr.getAttribute("font"));
            buttons.insert(std::make_pair(id, std::move(newButton)));
        }
        else if(name.compare("content") == 0){
            id = curr.getAttribute("name");
            SDL_Rect mRect;
            mRect.x = atoi(curr.getAttribute("x"));
            mRect.y = atoi(curr.getAttribute("y"));
            mRect.h = atoi(curr.getAttribute("h"));
            mRect.w = atoi(curr.getAttribute("w"));
            beatOff::cNovelList newNovels(&mRect);
            beatOff::cReader newReader(&mRect);
            beatOff::cNovelDetails newDetails(&mRect);
            mNovelList = &newNovels;
            mReader = &newReader;
            mNovelDetails = &newDetails;
        }
        else if(name.compare("colour") == 0){
            id = curr.getAttribute("name");
            SDL_Color newColour;
            newColour.r = atoi(curr.getAttribute("r"));
            newColour.g = atoi(curr.getAttribute("g"));
            newColour.b = atoi(curr.getAttribute("b"));
            newColour.a = atoi(curr.getAttribute("a"));
            colours.insert(std::make_pair(id, std::move(newColour)));
        }
        else{
            printf("Invalid menu object type! %s\n", name.c_str());
        }
    }
}

bool cMain::run(){
    bool rVal = 1;
    bool running = 1;
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
    printf("Runtime = %d", SDL_GetTicks() - startRunTime);
    return rVal;
}

void cMain::update(){
    
}

void cMain::render(){
    /* Clear the screen with the background colour */
    auto found = colors.find("clear");
    SDL_SetRenderDrawColor(mRenderer, found->r, found->g, found->b, found->a);
    SDL_RenderClear(mRenderer);
    /* Draw the content first */
    switch(whereAt){
        case settings:
            /* Not done yet. You mad? */
            break;
        
        case novelList:
            mNovelList->render(mRenderer);
            break;

        case novelDetails:
            /* Not done yet. You mad? */
            break;

        case reader:
            mReader->render(mRenderer);
            break;
        
        case dlList:
            /* Not done yet. You mad? */
            break;

        default:
            setError("Stuck at render. Invalid whereAt");
            break;
    }
    /* Draw the "interface" over the content. Interface will ALWAYS only
     * consist of the objects below (no dynamic loading etc...) */
    switch(whereAt){
        case settings:
            image["settings-selected"].render(mRenderer);
            buttons["novelList"].deselect();
            buttons["novelList"].render(mRenderer);
            buttons["reader"].deselect();
            buttons["reader"].render(mRenderer);
            image["downloads"].render(mRenderer);
            break;

        case novelList:
            image["settings"].render(mRenderer);
            buttons["novelList"].select();
            buttons["novelList"].render(mRenderer);
            buttons["reader"].deselect();
            buttons["reader"].render(mRenderer);
            image["downloads"].render(mRenderer);
            break;

        case novelDetails:
            image["settings"].render(mRenderer);
            buttons["novelList"].select();
            buttons["novelList"].render(mRenderer);
            buttons["reader"].deselect();
            buttons["reader"].render(mRenderer);
            image["downloads"].render(mRenderer);
            break;

        case reader:
            image["settings"].render(mRenderer);
            buttons["novelList"].deselect();
            buttons["novelList"].render(mRenderer);
            buttons["reader"].select();
            buttons["reader"].render(mRenderer);
            image["downloads"].render(mRenderer);
            break;

        case dlList:
            image["settings"].render(mRenderer);
            buttons["novelList"].deselect();
            buttons["novelList"].render(mRenderer);
            buttons["reader"].deselect();
            buttons["reader"].render(mRenderer);
            image["downloads-selected"].render(mRenderer);
            break;

        default:
            setError("Stuck at render. Invalid whereAt");
            break;
    }
    /* Display the image \0/ */
    SDL_RenderPresent(mRenderer);
    /* Come again to render more shit :D */
}
