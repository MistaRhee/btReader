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
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_file(mainPageFileName.c_str());
        if(res){
            pugi::xml_node category = doc.child("api").child("query").child("cagetorymembers");
            for(auto cmNode: category.children("cm")){
                tempStr = cmNode.attribute("title").value();
                novelDB[convTitle(tempStr)] = std::make_pair("","");
            }
        }
        else{
            /* XML Failed to load! */
            std::string e = currentDateTime() + " [database.cpp] Load config error! ";
            e += "manifest.db could not be parsed. Error: ";
            e += res.description();
            remove(mainPageFileName.c_str()); //Clean useless junk
            throw(e);
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
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_file("data/novels.db");
        pugi::xml_node mainNode = doc.child("novellist");
        if(res){
            for(auto newEntry: mainNode.children("novel")){
                novelDB[newEntry.attribute("title").value()] = std::make_pair(newEntry.attribute("location").value(), newEntry.attribute("revid").value());
            }
            int nodeSize = std::distance(mainNode.children("novel").begin(), mainNode.children("novel").end());
            if(novelDB.size() != nodeSize){
                std::string mWarning = currentDateTime();
                mWarning +=  ": [database.cpp] - Mismatch in numbers! \nRebuilding the database from scratch! Size: ";
                mWarning += std::to_string(novelDB.size());
                mWarning += " Novel List Size: ";
                mWarning += std::to_string(nodeSize);
                printf("%s \n", mWarning.c_str());
                rVal = 0;
            }
        }
        else{
            /* XML Failed to load! */
            std::string e = currentDateTime() + " [database.cpp] Load config error! ";
            e += "manifest.db could not be parsed. Error: ";
            e += res.description();
            throw(e);
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
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_file(fileName.c_str());

        if(res){
            pugi::xml_node rootNode = doc.child("api");
            if(original.compare(rootNode.child("query").child("novels").child("novel").child("revisions").child("rev").attribute("revid").value()) != 0) rVal = 0;
        }
        else{
            /* XML Failed to load! */
            std::string e = currentDateTime() + " [database.cpp] Load config error! ";
            e += "manifest.db could not be parsed. Error: ";
            e += res.description();
            remove(fileName.c_str());
            throw(e);
        }
        remove(fileName.c_str());
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
    std::map<std::string, std::pair<std::string, std::string> > tempNovelDB;
    printf("Updating the database \n");
    cHttpd stream1;
    std::string tempFile = tempLoc+generateRandomName(50);
    while(fileExists(tempFile)){
        tempFile = tempLoc+generateRandomName(50);
    }
    std::string novelName;
    stream1.download(domain+novelList, tempFile);
    try{
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_file(tempFile.c_str());
        if(res){
            pugi::xml_node category = doc.child("api").child("query").child("categorymembers");
            for(auto cm: category.children("cm")){
                novelName = cm.attribute("title").value();
                if(novelDB.count(novelName) > 0){
                    auto found = novelDB.find(novelName);
                    if(found->second.second.size() > 0){ //If the details for this novel has been DLed already, check if there is update and update the page
                        if(hasNew(novelName)){ //More recent version of what was already DLed
                            tempNovelDB[novelName] = getNovelDetails(novelName);
                            remove(found->second.first.c_str()); //Remove the old file
                        }
                    }
                    else tempNovelDB[novelName] = getNovelDetails(novelName);//New novel, just get the details
                }
            }
        }
        else{
            /* XML Failed to load! */
            std::string e = currentDateTime() + " [database.cpp] Load config error! ";
            e += "manifest.db could not be parsed. Error: ";
            e += res.description();
            throw(e);
        }
        novelDB.clear(); //To prevent removed novels from staying (i.e. abiding by BT rules)
        novelDB = tempNovelDB;
    }
    catch(mException& e){
        setError(e.what());
    }
    printf("%s: [database.cpp] - Finished updating the database \n", currentDateTime().c_str());
}

void cMain::replaceDatabase(){
    printf("Replacing Database! \n");
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child();
    root.set_name("novellist");

    int count = 0;
    for(auto it = novelDB.begin(); it != novelDB.end(); ++it){
        pugi::xml_node newEntry = root.append_child();
        newEntry.set_name("novel");
        newEntry.append_attribute("title") = it->first.c_str();
        newEntry.append_attribute("location") = it->second.first.c_str();
        newEntry.append_attribute("revid") = it->second.second.c_str();
        count++;
    }

    if(count != novelDB.size()){
        printf("%s: [database.cpp] - An error has occured when replacing the old database! Mismatch in numbers \n", currentDateTime().c_str());
    }
    else{
        if(!doc.save_file("data/novels.db")){
            printf("%s: [database.cpp] - An error has occured when writing to data/novels.db! Aborting! \n");
        }
    }
}

std::pair<std::string, std::string> cMain::getNovelDetails(std::string title){ //Returning the filename in combination with the revID
    try{
        std::string tempFile;
        std::string tempFile2;
        std::string novelStore;
        std::string revID;
        std::string progress;
        int exist;
        printf("%s: [database.cpp] - Getting Novel Details for %s. \n", currentDateTime().c_str(), title.c_str());
        cHttpd mDownload;
        cWikiParser mParser;
        tempFile = "data/temp/"+generateRandomName(50);
        tempFile2 = tempFile + "2";
        while(fileExists(tempFile)||fileExists(tempFile2)){
            tempFile = "data/temp/"+generateRandomName(50);
            tempFile2 = tempFile + "2";
        }
        mDownload.download(domain+pageDetail+title, tempFile);
        printf("%s: [database.cpp] - Page saved to %s. \n", currentDateTime().c_str(), tempFile.c_str());
        printf("%s: [database.cpp] - Extracting wiki text... \n", currentDateTime().c_str());

        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_file(tempFile.c_str());
        if(res){
            pugi::xml_node mainNode = doc.child("api");
            pugi::xml_node parseNode = mainNode.child("parse");
            pugi::xml_node linksNode = parseNode.child("links");

            revID = parseNode.attribute("revid").value();
            FILE*fout = fopen(tempFile.c_str(), "w+");
            FILE*fexist = fopen(tempFile2.c_str(), "w+");
            fprintf(fout, "%s", parseNode.child("wikitext").text().as_string());
            fclose(fout);

            int links = std::distance(linksNode.begin(), linksNode.end());	//get the number of links and send it through.
            fprintf(fexist, "%d \n", links);
            for(auto childLink: linksNode.children()){
                if(childLink.attribute("exists")) exist = 1;
                else exist = 0;
                fprintf(fexist, "%s|%d \n", childLink.text().as_string(), exist);
            }
            fclose(fexist);

            printf("%s: [database.cpp] - Extraction complete! \n", currentDateTime().c_str());
            novelStore = "data/novels/"+generateRandomName(50);
            while(fileExists(novelStore)){
                novelStore = "data/novels/"+generateRandomName(50);
            }
            printf("%s: [database.cpp] - Cleaning novel! Sorry, can't print the name of the file to be saved to due to copyright issues\n", currentDateTime().c_str());
            mParser.cleanNovel(tempFile, tempFile2, novelStore);
            printf("%s: [database.cpp] - Cleaned page stored in %s. \n", currentDateTime().c_str(), novelStore.c_str());
            printf("%s: [database.cpp] - Deleting temp file \n", currentDateTime().c_str());
            remove (tempFile.c_str());
            remove (tempFile2.c_str());
            return std::make_pair(novelStore, revID);
        }
        else{
            /* XML Failed to load! */
            std::string e = currentDateTime() + " [database.cpp] Load config error! ";
            e += "manifest.db could not be parsed. Error: ";
            e += res.description();
            throw(e);
        }
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
