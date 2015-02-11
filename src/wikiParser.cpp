#include "btreader.hpp"

#define SYNOPSIS 0
#define VOLUMES 1
#define CHAPTERS 2

void cWikiParser::cleanNovel(const std::string inFile, const std::string outFile){
    FILE*fin = fopen(inFile.c_str(), "r");
    FILE*fout = fopen(outFile.c_str(), "w+");
    char buffer[4096];
    XMLNode mainNode = XMLNode::createXMLTopNode("node");
    XMLNode infoNode = mainNode.addChild("info");
    std::string synopsisText;
    std::string tempStr;
    int status = 0;
    bool found = 0;
    while(true){
        fgets(buffer, 4096, fin);
        switch(status){
            case SYNOPSIS:
                if(buffer[0] == '=' and buffer[1] == '='){
                    tempStr = buffer;
                    tempStr.erase(0, 2);
                    tempStr.erase(tempStr.end()-2, tempStr.end());
                }
                while(true){
                    fgets(buffer, 4096, fin);
                    if(buffer[0] == '='){
                        break;
                    }
                    else{
                        synopsisText += buffer;
                        synopsisText += '\n';
                    }
                }
                XMLNode synopsis = mainNode.createChild("synopsis");
                synopsis.addText(synopsisText.c_str());
                synopsisText.clear();
                status = VOLUMES;
                break;
            case VOLUMES:
                if(!found){
                    if(buffer[0] == '=' and buffer[1] == '=' and buffer[2] != '='){
                        std::string word;
                        tempStr = buffer;
                        tempStr.erase(0, 2);
                        tempStr.erase(tempStr.end()-2, tempStr.end());
                        for(int i = 0, j = tempStr.size(); i < j; i++){
                            if(tempStr[i] == ' '){
                                if(word.compare("by") == 0){
                                    found = 1;
                                }
                                word.clear();
                                break;
                            }
                            else{
                                word += tempStr[i];
                            }
                        }
                    }
                }
                else{
                    
                }
                break;
            case CHAPTERS:
                asdf
                break;
            default:
                printf("You really shouldn't be in here you know! \n");
                break;
    }
}

std::string cWikiParser::getError(){
    return error;
}

std::string cWikiParser::setError(std::string inError){
    error = inError;
}
