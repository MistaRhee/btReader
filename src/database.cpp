#include "btReader.hpp"

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
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
    try{
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
            novelDB[convTitle(tempStr)] = std::make_pair("", "");
        }
        remove(mainPageFileName.c_str());
        updateDatabase();
    }
    catch(mException& e){
        setError(e.what());
    }
}

bool cMain::readDatabase(){ 
    bool rVal = 1;
    try{
        XMLNode mainNode = XMLNode::openFileHelper("data/novels.db", "novellist");
        for(int i = 0, j = mainNode.nChildNode("novel"); i < j; i++){
            XMLNode newEntry = mainNode.getChildNode("novel", i);
            novelDB[newEntry.getAttribute("title")] = std::make_pair(newEntry.getAttribute("location"), newEntry.getAttribute("revid"));
        }
        if(novelDB.size() != mainNode.nChildNode("novel")){
            std::string mWarning = currentDateTime();
            mWarning +=  ": [database.cpp] - Mismatch in numers! \nRebuilding the database from scratch! Size: ";
            mWarning += std::to_string(novelDB.size());
            mWarning += " Novel List Size: ";
            mWarning += std::to_string(mainNode.nChildNode("novel"));
            printf("%s \n", mWarning.c_str());
            rVal = 0;
        }
    }
    catch(mException& e){
        setError(e.what());
        std::string mError = currentDateTime();
        mError += ": Error - There was an exception! \n";
        mError += e.what();
        printf("%s \n", mError.c_str());
        rVal = 0;
    }
    return rVal;

}

bool cMain::hasNew(const std::string title){
    bool rVal = 1;
    try{
        cHttpd newDl;
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
    }
    catch(mException& e){
        setError(e.what());
        std::string mError = currentDateTime();
        mError += ": Error - There was an exception \n";
        mError += e.what();
        printf("%s \n", mError.c_str());
        rVal = 0;
    }
    return rVal;
}

void cMain::updateDatabase(){
    printf("Updating the database \n");
    cHttpd stream1;
    std::string tempFile = tempLoc+generateRandomName(50);
    while(fileExists(tempFile)){
        tempFile = tempLoc+generateRandomName(50);
    }
    std::string novelName;
    stream1.download(domain+novelList, tempFile);
    try{
        XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
        XMLNode queryNode = mainNode.getChildNode("query");
        XMLNode categoryMembers = queryNode.getChildNode("categorymembers");
        for(int i = 0, j = categoryMembers.nChildNode("cm"); i < j; i++){
            novelName = categoryMembers.getChildNode("cm", i).getAttribute("title");
            novelName = convTitle(novelName);
            if(novelDB.count(novelName) > 0){
                auto found = novelDB.find(novelName);
                if(found->second.second.size() > 0){
                    if(hasNew(novelName)){ //The page has been updated (i.e. there is an extra novel)
                        std::pair<std::string, std::string> mDetails = getNovelDetails(novelName);
                        novelDB[novelName] = mDetails;
                    }
                }
                else{
                    std::pair<std::string, std::string> mDetails = getNovelDetails(novelName);
                    novelDB[novelName] = mDetails;
                }
            }
            else{
                std::pair<std::string, std::string> mDetails = getNovelDetails(novelName);
                novelDB[novelName] = mDetails;
            }
        }
    }
    catch(mException& e){
        setError(e.what());
    }
    printf("%s: [database.cpp] - Finished updating the database \n", currentDateTime().c_str());
}

void cMain::replaceDatabase(){
    printf("Replacing Database! \n");
    XMLNode mainNode = XMLNode::createXMLTopNode("novellist");
    int count = 0;
    for(std::map<std::string, std::pair<std::string, std::string> >::iterator it = novelDB.begin(); it != novelDB.end(); ++it){
        XMLNode newEntry = mainNode.addChild("novel");
        newEntry.addAttribute("title", it->first.c_str());
        newEntry.addAttribute("location", it->second.first.c_str());
        newEntry.addAttribute("revid", it->second.second.c_str());
        count ++;
    }
    if(count != novelDB.size()){
        printf("%s: [database.cpp] - An error has occured when replacing the old database! Mismatch in numbers \n", currentDateTime().c_str());
    }
    else{
        char* t;
        t = mainNode.createXMLString(true);
        fprintf(fopen("data/novels.db", "w+"), "%s\n", t);
        free(t);
        printf("%s: [database.cpp] - Database successfully replaced! \n", currentDateTime().c_str());
    }
}

std::pair<std::string, std::string> cMain::getNovelDetails(std::string title){ //Returning the filename in combination with the revID
    try{
        std::string tempFile;
        std::string novelStore;
        std::string revID;
        std::string progress;
        printf("%s: [database.cpp] - Getting Novel Details for %s. \n", currentDateTime().c_str(), title.c_str());
        cHttpd mDownload;
        cWikiParser mParser;
        tempFile = "data/temp/"+generateRandomName(50);
        while(fileExists(tempFile)){
            tempFile = "data/temp/"+generateRandomName(50);
        }
        mDownload.download(domain+pageDetail+title, tempFile);
        printf("%s: [database.cpp] - Page saved to %s. \n", currentDateTime().c_str(), tempFile.c_str());
        printf("%s: [database.cpp] - Extracting wiki text... \n", currentDateTime().c_str());
        XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
        XMLNode parseNode = mainNode.getChildNode("parse");
        revID = parseNode.getAttribute("revid");
        FILE*fout = fopen(tempFile.c_str(), "w+");
        fprintf(fout, "%s", parseNode.getChildNode("wikitext").getText());
        fclose(fout);
        printf("%s: [database.cpp] - Extraction complete! \n", currentDateTime().c_str());
        novelStore = "data/novels/"+generateRandomName(50);
        while(fileExists(novelStore)){
            novelStore = "data/novels/"+generateRandomName(50);
        }
        printf("%s: [database.cpp] - Cleaning novel! Sorry, can't print the name of the file to be saved to due to copyright issues\n", currentDateTime().c_str());
        mParser.cleanNovel(tempFile, novelStore);
        printf("%s: [database.cpp] - Cleaned page stored in %s. \n", currentDateTime().c_str(), novelStore.c_str());
        printf("%s: [database.cpp] - Deleting temp file \n", currentDateTime().c_str());
        remove (tempFile.c_str());
        remove (tempFile2.c_str());
        return std::make_pair(novelStore, revID);
    }
    catch(mException& e){
        std::string mError = currentDateTime();
        mError += ": [database.cpp] - ";
        mError += e.what();
        setError(e.what());
        printf("%s\n", mError.c_str());
        return std::make_pair("", "");
    }
}
