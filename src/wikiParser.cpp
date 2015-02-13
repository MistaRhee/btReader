#include "btreader.hpp"

#define END -1
#define SYNOPSIS 0
#define VOLUMES 1

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

std::string cWikiParser::generateRandomName(int length){ 
    srand(time(NULL));     
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";       
    std::string rVal;      
    for(int i = 0, j = strlen(aCharacters); i < length; i++){      
        rVal += aCharacters[rand()%j];     
    }      
    return rVal;       
}

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
                    XMLNode newVolume = mainNode.addChild("volume");
                    if(buffer[0] == '=' and buffer[1] == '=') {
                        if(buffer[2] == '='){
                            tempStr = buffer;
                            tempStr.erase(0, 3);
                            std::string title;
                            for(int i = 0, j = tempStr.size(); i < j; i++){
                                if(tempStr[i] == '(' or tempStr[i] == '='){
                                    newVolume.addAttribute("title", volumeTitle.c_str());
                                    volumeTitle.clear();
                                }
                                else{
                                    volumeTitle += tempStr[i];
                                }
                            }
                            fgets(buffer, 4096, fin);
                            std::string fileName;
                            cGetImage newImageGrab;
                            std::string savedTo = newImageGrab.getImage(fileName);
                            newVolume.addAttribute("image", savedTo.c_str());
                            while(true){
                                XMLNode chapterNode = newVolume.addChild("chapter");
                                fgets(buffer, 4096, fin);
                                std::string title;
                                if(buffer[0] == ':'){
                                    bool grabbing = 0;
                                    for(int i = 1, j = strlen(buffer); i < j; i++){
                                        if(grabbing){
                                            if(buffer[i] == ']'){
                                                break;
                                            }
                                            else{
                                                title += buffer[i];
                                            }
                                        }
                                        if(buffer[i] == '|'){
                                            grabbing = 1;
                                        }
                                    }
                                    chapterNode.addAttribute("title", title.c_str());
                                    title.clear();
                                    title = "data/novels/"+generateRandomName(25);
                                    while(fileExists(title)){
                                        title = "data/novels/"+generateRandomName(25);
                                    }
                                    chapterNode.addAttribute("location", title.c_str()); //The chapter will be saved here, it doesn't mean that it will actually have content stored there... That will come later.
                                    fprintf(fopen(title.c_str(), "w+"), "<chapter dl=\"no\"/>\n");
                                }
                                else{
                                    break;
                                }
                            }

                        }
                        else{
                            status = END;
                        }
                    }             
                }
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
    char *t=mainNode.createXMLString(true);
    fprintf(fout, "%s \n", t);
    free(t);
}

std::string cWikiParser::getError(){
    return error;
}

std::string cWikiParser::setError(std::string inError){
    error = inError;
}
