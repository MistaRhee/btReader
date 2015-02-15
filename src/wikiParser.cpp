#include "btreader.hpp"

static const int EXIT = -1;
static const int SYNOPSIS = 0;
static const int VOLUMES = 1;

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
    XMLNode mainNode = XMLNode::createXMLTopNode("novel");
    XMLNode infoNode = mainNode.addChild("info");
    std::string tempStr;
    int status = SYNOPSIS;
    bool found = 0;
    while(true){
        if(feof(fin)){
            break;
        }
        fgets(buffer, 4096, fin);
        switch(status){
            case SYNOPSIS:
                {
                    std::string synopsisText;
                    if(buffer[0] == '=' and buffer[1] == '='){
                        tempStr = buffer;
                        tempStr.erase(0, 2);
                        tempStr.erase(tempStr.end()-3, tempStr.end());
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
                        XMLNode synopsis = mainNode.addChild("synopsis");
                        synopsis.addText(synopsisText.c_str());
                        synopsisText.clear();
                        status = VOLUMES;
                    }
                    break;
                }
            case VOLUMES:
                {
                    if(!found){
                        if(buffer[0] == '=' and buffer[1] == '=' and buffer[2] != '='){
                            std::string word;
                            tempStr = buffer;
                            tempStr.erase(0, 2);
                            tempStr.erase(tempStr.end()-3, tempStr.end());
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
                                        break;
                                    }
                                    word.clear();
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
                                XMLNode newVolume = mainNode.addChild("volume");
                                tempStr = buffer;
                                tempStr.erase(0, 3);
                                std::string volumeTitle;
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
                                bool skipOne = 1;
                                if(buffer[0] == '[' and buffer[1] == '['){
                                    for(int i = 2, j = strlen(buffer); i < j; i++){
                                        if(buffer[i] == '|'){
                                            break;
                                        }
                                        else{
                                            fileName += buffer[i];
                                        }
                                    }
                                    skipOne = 0;
                                }
                                cGetImage newImageGrab;
                                std::string savedTo = newImageGrab.getImage(fileName);
                                newVolume.addAttribute("image", savedTo.c_str());
                                while(true){
                                    XMLNode chapterNode = newVolume.addChild("chapter");
                                    if(skipOne){
                                        fgets(buffer, 4096, fin);
                                        skipOne = 0;
                                    }
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
                                        title = "data/novels/"+generateRandomName(50);
                                        while(fileExists(title)){
                                            title = "data/novels/"+generateRandomName(50);
                                        }
                                        chapterNode.addAttribute("location", title.c_str()); //The chapter will be saved here, it doesn't mean that it will actually have content stored there... That will come later.
                                        chapterNode.addAttribute("dl", "no");
                                        chapterNode.addAttribute("revid", "");
                                    }
                                    else{
                                        break;
                                    }
                                }

                            }
                            else{
                                status = EXIT;
                            }
                        }             
                    }
                    break;
                }
            case EXIT:
                printf("Time to go! \n");
                break;
            default:
                printf("You really shouldn't be in here you know! \n");
                break;
        }
        if(status == EXIT){
            break;
        }
    }
    char *t=mainNode.createXMLString(true);
    fprintf(fout, "%s \n", t);
    fclose(fout);
    free(t);
}

void cWikiParser::cleanChapter(const std::string in, const std::string out){
    printf("Bleh not done yet! \n");
}

std::string cWikiParser::getError(){
    return error;
}

void cWikiParser::setError(std::string inError){
    error = inError;
}
