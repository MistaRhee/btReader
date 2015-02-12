#include "btreader.hpp"

#define END -1
#define SYNOPSIS 0
#define VOLUMES 1
#define CHAPTERS 2

void cWikiParser::cleanNovel(const std::string inFile, const std::string outFile){
    FILE*fin = fopen(inFile.c_str(), "r");
    FILE*fout = fopen(outFile.c_str(), "w+");
    char buffer[4096];
    XMLNode mainNode = XMLNode::createXMLTopNode("node");
    XMLNode infoNode = mainNode.addChild("info");
    std::string tempStr;
    int status = 0;
    bool found = 0;
    while(true){
        fgets(buffer, 4096, fin);
        switch(status){
            case SYNOPSIS:
                std::string synopsisText;
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
                                    std::string title;
                                    for(int k = 0; k < i-1; k++){
                                        title += tempStr[k];
                                    }
                                    infoNode.addAttribute("title", title.c_str());
                                    title.clear();
                                    for(int k = i; k < j; k++){
                                        title += tempStr[k];
                                    }
                                    infoNode.addAttribute("author", title.c_str());
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
                    if(buffer[0] == '=' and buffer[1] == '=') {
                        if(buffer[2] == '='){
                            tempStr = buffer;
                            tempStr.erase(0, 3);
                            for(int i = 0, j = tempStr.size(); i < j; i++){
                                
                            }

                        }
                        else{
                            status = END;
                        }
                    }             
                }
                break;
            case CHAPTERS:
                asdf
                break;
            case EXIT:
                printf("Time to go! \n");
                break;
            default:
                printf("You really shouldn't be in here you know! \n");
                break;
        }
        if(status == END){
            break;
        }
    }
}

std::string cWikiParser::getError(){
    return error;
}

std::string cWikiParser::setError(std::string inError){
    error = inError;
}
