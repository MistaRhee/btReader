#include "btreader.hpp"

//Sample page name: Absolute_Duo:Volume_1_Chapter_1

cMain::cMain(){
    preComp();
    currThreads = 1;
}

void cMain::preComp(){
    if(!fileExists()){
        printf("No database found! Rebuilding novel list from site! \n");
        createDatabase();
    }
    else readDatabase();
}

void cMain::setError(std::string mError){
    error = mError;
}

std::string getError(){
    return error;
}

void cMain::createDatabase(){
    //This is the lengthy process of getting the novels when the program first
    //starts up. Just so the user doesn't have to do it themselves
    cHttpd stream1;
    std::string mainPageFileName = qDownload(domain+types, 16);
    std::string tempStr;
    FILE*fout = fopen("novels.db", "w+");
    XMLNode mainNode = XMLNode::openFileHeader(mainPageFileName.c_str(), "api");
    XMLNode queryNode = mainNode.getChildNode("categorymembers");
    for(int i = 0, j = queryNode.nChildNode("cm"); i < j; i++){
        tempStr = queryNode.getChildNode("categorymembers").getAttribute("name");
        fprintf(fout, "%s\n", convTitle(tempStr).c_str());
    }
}

std::string convTitle(std::string incoming){
    std::string output;
    for(int i = 0, j = incoming.size(); i < j; i++){
        if(incoming[i] == ' '){
            output += '_';
        }
        else output += incoming[i];
    }
    return output;
}
