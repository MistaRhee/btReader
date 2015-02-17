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
    mWindow = SDL_CreateWindow("btReader - By MistaRhee and NoOne2246", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_SHOWN);
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    currThreads = 1;
}

cMain::~cMain(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
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

std::string cMain::generateRandomName(int length){ 
    srand(time(NULL));     
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";       
    std::string rVal;      
    for(int i = 0, j = strlen(aCharacters); i < length; i++){      
        rVal += aCharacters[rand()%j];     
    }      
    return rVal;       
}

void cMain::createDatabase(){
    //This is the lengthy process of getting the novels when the program first
    //starts up. Just so the user doesn't have to do it themselves
    cHttpd stream1;
    std::string mainPageFileName = tempLoc+generateRandomName(50);
    while(fileExists(mainPageFileName)){
        mainPageFileName = tempLoc+generateRandomName(50);
    }
    stream1.download(domain+novelList, mainPageFileName);
    std::string tempStr;
    XMLNode mainNode = XMLNode::openFileHelper(mainPageFileName.c_str(), "api");
    XMLNode queryNode = mainNode.getChildNode("query");
    XMLNode categoryNode = queryNode.getChildNode("categorymembers");
    for(int i = 0, j = categoryNode.nChildNode("cm"); i < j; i++){
        tempStr = categoryNode.getChildNode("cm", i).getAttribute("title");
        novelDB.insert(std::make_pair(convTitle(tempStr), std::make_pair("", "")));
    }
    remove(mainPageFileName.c_str());
    updateDatabase();
}

bool cMain::readDatabase(){ 
    bool rVal = 0;
    XMLNode mainNode = XMLNode::openFileHelper("data/novels.db", "novelList");
    for(int i = 0, j = mainNode.nChildNode("novel"); i < j; i++){
        XMLNode newEntry = mainNode.getChildNode("novel", i);
        novelDB.insert(std::make_pair(newEntry.getAttribute("title"), std::make_pair(newEntry.getAttribute("location"), newEntry.getAttribute("revid"))));
    }
    if(novelDB.size() != mainnode.nChildNode("novel")){
        printf("An error has occurred when reading the database! Mismatch in numbers! \nRebuilding the database from scratch! \n");
        rVal = 1;
    }
    return rVal;
}

bool cMain::hasNew(const std::string title){
    cHttpd newDl;
    bool rVal = 1;
    const std::string original = novelDB.find(title)->second.second;
    std::string fileName = tempLoc+generateRandomName(50);
    while(fileExists(fileName)){
        fileName = tempLoc+generateRandomName(50);
    }
    newDl.download(domain+revID+title, fileName);
    XMLNode mNode = XMLNode::openFileHelper(fileName.c_str(), "api");
    if(original.compare(mNode.getChildNode("query").getChildNode("novels").getChildNode("novel").getChildNode("revisions").getChildNode("rev").getAttribute("revid"))!= 0){
        rVal = 0;
    }
    return rVal;
}

void cMain::updateDatabase(){
    cHttpd stream1;
    std::string tempFile = tempLoc+generateRandomName(50);
    while(fileExists(tempFile)){
        tempFile = tempLoc+generateRandomName(50);
    }
    std::string novelName;
    stream1.download(domain+novelList, tempFile);
    XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
    XMLNode queryNode = mainNode.getChildNode("query");
    XMLNode categoryMembers = queryNode.getChildNode("categorymembers");
    for(int i = 0, j = categoryMembers.nChildNode("cm"); i < j; i++){
        novelName = categoryMembers.getChildNode("cm", i).getAttribute("title");
        novelName = convTitle(novelName);
        if(novelDB.count(novelName) > 0){
            if(hasNew(novelName)){ //The page has been updated (i.e. there is an extra novel)
                std::map<std::string, std::pair<std::string, std::string> >::iterator it = novelDB.find(novelName);
                novelDB.erase(it);
                novelDB.insert(std::make_pair(novelName, getNovelDetails(novelName)));
            }
        }
        else{
            novelDB.insert(std::make_pair(novelName, getNovelDetails(novelName)));
        }
    }
}

void cMain::replaceDatabase(){
    XMLNode mainNode = XMLNode::createXMLTopNode("novellist");
    int count = 0;
    for(std::map<std::string, std::pair<std::string, std::string> >::iterator it; it != novelDB.end(); ++it){
        XMLNode newEntry = mainNode.addChildNode("novel");
        newEntry.addAttribute("title", it->first.c_str());
        newEntry.addAttribute("location", it->second.first.c_str());
        newEntry.addAttribute("revid", it->second.second.c_str());
        count ++;
    }
    if(count != novelDB.size()){
        printf("An error has occured when replacing the old database! Mismatch in numbers \n");
    }
    else{
        char* t;
        t = mainNode.createXMLString(true);
        fprintf(fopen("data/novels.db", "w+"), "%s\n", t);
        free(t);
        printf("Database successfully replaced! \n");
    }
}

bool cMain::run(){
    int startTick = SDL_GetTicks();
    createDatabase();
    /*
    while(mWindow != NULL){
        startTick = SDL_GetTicks();
        while(SDL_GetTicks() < startTick+FPS_CAP){
            processEvents();
            update();
        }
        render();
    }
    Will be uncommented when getting the database has been optimised
    */
    printf("Runtime = %d", SDL_GetTicks() - startTick);
    return 1;
}

std::pair<std::string, std::string> cMain::getNovelDetails(std::string title){ //Returning the filename in combination with the revID
    std::string tempFile;
    std::string novelStore;
    std::string revID;
    std::string progress;
    printf("Getting Novel Details for %s. \n", title.c_str());
    cHttpd mDownload;
    cWikiParser mParser;
    tempFile = "data/temp/"+generateRandomName(50);
    while(fileExists(tempFile)){
        tempFile = "data/temp/"+generateRandomName(50);
    }
    mDownload.download(domain+pageDetail+title, tempFile);
    printf("Page saved to %s. \n", tempFile.c_str());
    printf("Extracting wiki text... \n");
    XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
    XMLNode parseNode = mainNode.getChildNode("parse");
    revID = parseNode.getAttribute("revid");
    FILE*fout = fopen(tempFile.c_str(), "w+");
    fprintf(fout, "%s", parseNode.getChildNode("wikitext").getText());
    fclose(fout);
    printf("Extraction complete! \n");
    novelStore = "data/novels/"+generateRandomName(50);
    while(fileExists(novelStore)){
        novelStore = "data/novels/"+generateRandomName(50);
    }
    printf("Cleaning novel! Sorry, can't print the name of the file to be saved to due to copyright issues\n");
    mParser.cleanNovel(tempFile, novelStore);
    printf("Cleaned page stored in %s. \n", novelStore.c_str());
    printf("Deleting old file \n");
    remove (tempFile.c_str());
    return std::make_pair(novelStore, revID);
}
