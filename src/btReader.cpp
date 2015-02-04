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
    currThreads = 1;
}

void cMain::preComp(){
    if(!fileExists("novels.db")){
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

void cMain::createDatabase(){
    //This is the lengthy process of getting the novels when the program first
    //starts up. Just so the user doesn't have to do it themselves
    cHttpd stream1;
    std::string mainPageFileName = stream1.qDownload(domain+novelList, 16);
    std::string tempStr;
    FILE*fout = fopen("novels.db", "w+");
    XMLNode mainNode = XMLNode::openFileHelper(mainPageFileName.c_str(), "api");
    XMLNode queryNode = mainNode.getChildNode("query");
    XMLNode categoryNode = queryNode.getChildNode("categorymembers");
    fprintf(fout, "%d\n", categoryNode.nChildNode("cm"));
    for(int i = 0, j = categoryNode.nChildNode("cm"); i < j; i++){
        tempStr = categoryNode.getChildNode("cm", i).getAttribute("title");
        fprintf(fout, "%s\n", convTitle(tempStr).c_str());
        printf("%s \n", tempStr.c_str());
    }
    fclose(fout);
    remove(mainPageFileName.c_str());
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

bool cMain::readDatabase(){
    bool rVal = 1;
    FILE*fin = fopen("novels.db", "w+");
    asdf
}

bool cMain::run(){
    return 1;
}
