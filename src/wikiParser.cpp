#include "btReader.hpp"

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
    bool processed = 1;
    while(true){
        if(feof(fin)){
            break;
        }
        if(processed){
            fgets(buffer, 4096, fin);
        }
        if(!processed){
            processed = 1;
        }
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
                        std::string fileName;
                        if(buffer[0] == '[' and buffer[1] == '['){
                            /* Get the link, check if it is an image. If it is,
                             * hold it just in case its useful
                             * ************************************************/
                            for(int i = 2, j = strlen(buffer); i < j; i++){
                                if(buffer[i] == '|' or ']'){
                                    break;
                                }
                                else{
                                    fileName += buffer[i];
                                }
                            }
                            fgets(buffer, 4096, fin);
                        }
                        if(buffer[0] == '=' and buffer[1] == '='){
                            if(buffer[2] == '='){
                                XMLNode newVolume = mainNode.addChild("volume");
                                printf("Adding Volume \n");
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
                                while(true){
                                    fgets(buffer, 4096, fin);
                                    std::string title;
                                    if(buffer[0] == ':' or buffer[0] == '*'){
                                        printf("Adding Chapter! \n");
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
                                        XMLNode chapterNode = newVolume.addChild("chapter"); //Moved here so it will only create a new chapter node if there is atually something worth grabbing
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
                                    else if(buffer[0] == '['){
                                        /* There is a link without indentataion
                                         * I'm just gonna assume its another
                                         * image
                                         */
                                        printf("Adding Image! \n");
                                        if(!newVolume.isAttributeSet("image")){
                                            for(int i = 1, j = strlen(buffer); i < j; i++){
                                                if(buffer[i] == '['){
                                                    /* Ignore useless
                                                     * characters */
                                                }
                                                if(buffer[i] == '|'){
                                                    break;
                                                }
                                                else{
                                                    fileName += buffer[i];
                                                }
                                            }
                                            cGetImage newImageGrab;
                                            std::string savedTo = newImageGrab.getImage(fileName);
                                            newVolume.addAttribute("image", savedTo.c_str());
                                        }
                                    }
                                    else if(strlen(buffer) == 1 or buffer[0] == '<' or buffer[0] == '\'' or buffer[0] == '&'){
                                        /* Ignore this, because it's just a
                                         * whitespace or HTML tag or a comment
                                         * etc...
                                         */
                                    }
                                    else{
                                        printf("Exiting adding chapters with buffer: %s \n", buffer);
                                        processed = 0;
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
                /* Time to go! */
                break;
            default:
                /* You really shouldn't be in here! */
                break;
        }
        if(status == EXIT){
            break;
        }
    }
    char* t = mainNode.createXMLString(true);
    fprintf(fout, "%s \n", t);
    fclose(fout);
    free(t);
}

void cWikiParser::cleanChapter(const std::string in, const std::string out){
    FILE*fin = fopen(in.c_str(), "r");
    char buffer[1000000];
    std::string text;
    while(true){
        if(!feof(fin)){
            break;
        }
        else{
            fgets(buffer, 1000000, fin);
            if(buffer[0] == '[' and buffer[1] == '['){
                std::string fileName;
                for(int i = 2;; i++){
                    if(buffer[i] == '|'){
                        break;
                    }
                    else{
                        fileName += buffer[i];
                    }
                }
                cGetImage newImage;
                text += "[[";
                text += newImage.getImage(fileName);
                text += "]]";
            }
            else{
                text += buffer;
                text += "\n";
            }
        }
    }
    fclose(fin);
    fprintf(fopen(out.c_str(), "w+"), "%s \n", text.c_str());
}

std::string cWikiParser::getError(){
    return error;
}

void cWikiParser::setError(std::string inError){
    error = inError;
}
