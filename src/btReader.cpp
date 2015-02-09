#include "btreader.hpp"

//Sample page name: Absolute_Duo:Volume_1_Chapter_1

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

cMain::cMain(){
    preComp();
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        printf("SDL could not initialize! SDL_Error: %s \n", SDL_GetError());
    }
    mWindow = SDL_CreateWindow("btReader - By MistaRhee and NoOne2246", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600);
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    currThreads = 1;
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
}

void cMain::setError(std::string mError){
    error = mError;
}

std::string cMain::getError(){
    return error;
}

std::string cMain::convTitle(std::string incoming){
    std::string output;
    for(int i = 0, j = incoming.size(); i < j; i++){
        if(incoming[i] == ' '){
            output += '_';
        }
        else output += incoming[i];
    }
    return output;
}

void cMain::createDatabase(){
    //This is the lengthy process of getting the novels when the program first
    //starts up. Just so the user doesn't have to do it themselves
    cHttpd stream1;
    std::string mainPageFileName = tempLoc+generateRandomName(16);
    stream1.download(domain+novelList, mainPageFileName);
    std::string tempStr;
    XMLNode mainNode = XMLNode::openFileHelper(mainPageFileName.c_str(), "api");
    XMLNode queryNode = mainNode.getChildNode("query");
    XMLNode categoryNode = queryNode.getChildNode("categorymembers");
    fprintf(fout, "%d\n", categoryNode.nChildNode("cm"));
    for(int i = 0, j = categoryNode.nChildNode("cm"); i < j; i++){
        tempStr = categoryNode.getChildNode("cm", i).getAttribute("title");
        novelDB.insert(std::make_pair(tempStr, std::make_pair("", "")));
    }
    fclose(fout);
    remove(mainPageFileName.c_str());
    updateDatabase();
}

bool cMain::readDatabase(){ //Note to self: Use try + catch for error handling maybe?
    bool rVal = 1;
    bool tick = 0, rev = 0;
    int n;
    char buffer[200];
    std::string title;
    std::string fileName;
    std::string revID;
    FILE*fin = fopen("novels.db", "r");
    fscanf(fin, "%d", &n);
    for(int i = 0; i < n; i++){
        title.clear();
        fileName.clear();
        revID.clear();
        tick = 0;
        rev = 0;
        fgets(buffer, 200, fin);
        for(int j = 0, k = strlen(buffer); j < k; j++){
            if(!tick && buffer[j] == ' '){
                tick = 1;
                continue;
            }
            if(tick && buffer[j] == ' '){
                rev = 1;
                continue;
            }
            if(tick && !rev){
                fileName += buffer[j];
            }
            if(rev){
                revID += buffer[j];
            }
            else title += buffer[j];
        }
        novelDB.insert(std::make_pair(title, std::make_pair(fileName, revID)));
    }
    fclose(fin);
    return rVal;
}

void cMain::updateDatabase(){
    cHttpd stream1;
    std::string tempFile = tempLoc+generateRandomName(16);
    std::string novelName;
    stream1.download(domain+novelList, tempFile);
    XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
    XMLNode queryNode = mainNode.getChildNode("query");
    XMLNode categoryMembers = queryNode.getChildNode("categorymembers");
    for(int i = 0, j = categoryMembers.nChildNode("cm"); i < j; i++){
        novelName = categoryMembers.getChildNode("cm", i).getAttribute("title");
        
    }
}

void cMain::replaceDatabase(){
    FILE*fout = fopen("data/novels.db", "w+");
    fprintf(fout, "%d\n", novelDB.size());
    int count = 0;
    for(std::map<std::string, std::pair<std::string, std::string> >::iterator it; it != novelDB.end(); ++it){
        fprintf(fout, "%s %s %s\n", it->first, it->second.first, it->second.second);
        count ++;
    }
    if(count != novelDB.size()) printf("An error has occured when replacing old database! Unsure of the issue \n");
    else printf("Old database has been successfully replaced! \n");
}

bool cMain::run(){
    int startTick = SDL_GetTicks();
    while(mWindow != NULL){
        startTick = SDL_GetTicks();
        while(SDL_GetTicks() < startTick+FPS_CAP){
            processEvents();
            update();
        }
        render();
    }
}

std::string cMain::getNovelDetails(std::string title){
    std::string tempFile;
    std::string novelStore;
    std::string progress;
    printf("Getting Novel Details for %s. \n", title.c_str());
    cHttpd mDownload;
    cWikiParser mParser;
    tempFile = "data/temp/"+generateRandomName();
    mDownload.download(domain+pageDetail+title, tempFile);
    printf("Page saved to %s. \n", titleName.c_str());
    printf("Extracting wiki text... \n");

    printf("Extraction complete! \n");
    novelStore = "data/novels"+generateRandomName();
    printf("Cleaning novel! Sorry, can't print the name of the file to be saved to due to copyright issues\n");
    mParser.clean(tempFile, novelStore);
    printf("Cleaned page stored in %s. \n", titleName.c_str());
    printf("Deleting old file \n");
    remove (tempFile.c_str());
    return novelStore;
}
