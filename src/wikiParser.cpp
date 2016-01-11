#include "btReader.hpp"

static const int EXIT = -1;
static const int SYNOPSIS = 0;
static const int VOLUMES = 1;

enum splitter_t{
    locate,
    description,
    spacer,
    novTitle,
    leave   
        //
};

inline bool fileExists (const std::string& name) {
    if(FILE *file = fopen(name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
        return false;
    }   
}

cWikiParser::cWikiParser(){
    std::string logLoc = "logs/";
    logLoc += "wikiParser.log";
    this->mLog = new __logger::cLogger(logLoc);
    logLoc.clear();
}

cWikiParser::cWikiParser(__logger::cLogger* mLog){
    this->mLog = mLog;
}

std::string cWikiParser::titleClean(const std::string title){
    std::string cleaned;
    for(int i = 0, j = title.size(); i<j; i++){
        if(title[i] == ' '){
            cleaned += '_';
        }else{
            cleaned += title[i];
        }
    }
    return cleaned;
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

void cWikiParser::cleanNovel(const std::string inFile, const std::string existFile, const std::string outFile){
    FILE*fin = fopen(inFile.c_str(), "r");
    FILE*fexist = fopen(existFile.c_str(), "r");
    char buffer[4096];

    pugi::xml_document doc;
    pugi::xml_node mainNode = doc.append_child("novel");
    pugi::xml_node infoNode = mainNode.append_child("info");

    std::string tempStr;
    int status = SYNOPSIS;
    bool found = 0;
    bool processed = 1;
    bool getting = 1;

    //get data out of fexist
    int numLinks = 0;
    fscanf(fexist, "%i", &numLinks);    //get the max number of links to pull out
    int availInt;
    std::map<std::string, int> availMap;//map with the string and availability

    for(int i = 0; i<numLinks;i++){             //pull string and info out
        fgets(buffer, 4096, fexist);
        std::string volStr;                 //temporary storage of string
        getting = true;
        for(int i = 0, j = strlen(buffer); i < j; i++){
            if(getting==true){
                if(buffer[i]!='|'){
                    volStr += buffer[i];
                }else{
                    getting = false;
                }
            }else{
                availInt = atoi(&buffer[i]);
                break;
            }
        }
        volStr = titleClean(volStr);
        availMap[volStr] = availInt;            //move data into map
        volStr.clear();
    }

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
                        pugi::xml_node synopsis = mainNode.append_child("synopsis");
                        synopsis.text().set(synopsisText.c_str()); //Safe to use this because there will only be one text field anyway
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
                                        pugi::xml_attribute tempAtt = infoNode.append_attribute("title");
                                        tempAtt.set_value(title.c_str());
                                        title.clear();
                                        for(int k = i; k < j; k++){
                                            title += tempStr[k];
                                        }
                                        tempAtt = infoNode.append_attribute("author");
                                        tempAtt.set_value(title.c_str());
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
                                pugi::xml_node newVol = mainNode.append_child("volume");
                                tempStr = buffer;
                                tempStr.erase(0, 3);
                                std::string volumeTitle;
                                for(int i = 0, j = tempStr.size(); i < j; i++){
                                    if(tempStr[i] == '(' or tempStr[i] == '='){
                                        pugi::xml_attribute tAtt = newVol.append_attribute("title");
                                        tAtt.set_value(volumeTitle.c_str());
                                        volumeTitle.clear();
                                        break;
                                    }
                                    else{
                                        volumeTitle += tempStr[i];
                                    }
                                }
                                while(true){
                                    fgets(buffer, 4096, fin);
                                    std::string title;
                                    std::string chapName;
                                    if(buffer[0] == ':' or buffer[0] == '*'){
                                        splitter_t grabbing = locate;
                                        for(int i = 1, j = strlen(buffer); i < j; i++){
                                            switch(grabbing){
                                                case locate:
                                                    if(buffer[i] == '['&&buffer[i+1] != '['){
                                                        grabbing = description;
                                                    }
                                                    break;

                                                case description:
                                                    if(buffer[i] == '|'){
                                                        grabbing = spacer;
                                                    }
                                                    else if (buffer[i]==']'){
                                                        title = chapName;
                                                        grabbing = leave;
                                                    }
                                                    else{
                                                        chapName += buffer[i];
                                                    }
                                                    break;

                                                case spacer:
                                                    if(buffer[i]==' '){
                                                        while(buffer[i]==' '){
                                                            i++;
                                                        }
                                                    }
                                                    title += buffer[i];
                                                    grabbing = novTitle;
                                                    break;

                                                case novTitle:
                                                    if(buffer[i] == ']'){
                                                        grabbing = leave;
                                                    }
                                                    else{
                                                        title += buffer[i];
                                                    }
                                                    break;

                                                default:
                                                    this->mLog->log("[wikiParser.cpp] Error: Enum somehow got into default");
                                            }
                                            if(grabbing == leave){
                                                break;
                                            }
                                        }
                                        pugi::xml_node chapterNode = newVol.append_child("chapter"); //Moved here so it will only create a new chapter node if there is atually something worth grabbing
                                        pugi::xml_attribute tempAtt = chapterNode.append_attribute("title");
                                        tempAtt.set_value(title.c_str());
                                        title.clear();
                                        title = "data/novels/"+generateRandomName(50);
                                        while(fileExists(title)){
                                            title = "data/novels/"+generateRandomName(50);
                                        }

                                        tempAtt = chapterNode.append_attribute("location");
                                        tempAtt.set_value(title.c_str()); //The chapter will be saved here, it doesn't mean that it will actually have content stored there... That will come later.
                                        tempAtt = chapterNode.append_attribute("dl");
                                        tempAtt.set_value("no");
                                        tempAtt = chapterNode.append_attribute("revid");
                                        tempAtt.set_value(""); //Not really needed because default to be nothing
                                        //check whether the link is available.
                                        chapName = titleClean(chapName);
                                        auto it = availMap.find(chapName);
                                        if(it != availMap.end()){
                                            if(it->second==1){
                                                tempAtt = chapterNode.append_attribute("available");
                                                tempAtt.set_value("1");
                                            }else{
                                                tempAtt = chapterNode.append_attribute("available");
                                                tempAtt.set_value("0");
                                            }
                                        }else{
                                            std::string err = "[wikiParser.cpp] Warning: Unable to locate ";
                                            err += chapName + " within the map. Set as not available for now \n";
                                            this->mLog->log(err);
                                            tempAtt = chapterNode.append_attribute("available");
                                            tempAtt.set_value("0");
                                        }
                                    }
                                    else if(buffer[0] == '['){
                                        /* There is a link without indentataion
                                         * I'm just gonna assume its another
                                         * image
                                         */
                                        fileName.clear();
                                        bool shouldGrab = 0;
                                        if(!newVol.attribute("image")){
                                            for(int i = 1, j = strlen(buffer); i < j; i++){
                                                if(buffer[i] == '['){
                                                    /* Iss 13 fix */
                                                    shouldGrab = 1;
                                                    i++; //Skip grabbing the second bracket too
                                                }
                                                if(buffer[i] == '|'){
                                                    break;
                                                }
                                                else if(shouldGrab){
                                                    fileName += buffer[i];
                                                }
                                                else{
                                                    /* Is external link
                                                     * (According to iss#13)
                                                     */
                                                    break;
                                                }
                                            }
                                            /* Check the first four characters
                                             * of my image file is "File"
                                             */
                                            std::string tempString(fileName.begin(), fileName.begin()+4);
                                            std::string tempString2(fileName.begin(), fileName.begin()+5);
                                            if(tempString == "File" or tempString2 == "Image"){
                                                this->mLog->log(std::string("[wikiParser.cpp] Info: Calling cGetImage with ") + fileName);
                                                cGetImage newImageGrab(this->mLog);
                                                std::string savedTo = newImageGrab.getImage(fileName);
                                                pugi::xml_attribute tempAtt = newVol.append_attribute("image");
                                                tempAtt.set_value(savedTo.c_str());
                                            }
                                            else{
                                                std::string err = "[wikiParser.cpp] Warning: Invalid image name ";
                                                err += tempString + " -> Ignoring";
                                                this->mLog->log(err);
                                                break;
                                            }
                                        }
                                    }
                                    else if(strlen(buffer) == 1 or buffer[0] == '<' or buffer[0] == '\'' or buffer[0] == '&'){
                                        /* Ignore this, because it's just a
                                         * whitespace or HTML tag or a comment
                                         * etc...
                                         */
                                    }
                                    else{
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
    doc.save_file(outFile.c_str());
    fclose(fexist);
}

void cWikiParser::cleanChapter(const std::string in, const std::string out){
    FILE*fin = fopen(in.c_str(), "r");
    char buffer[1000000];
    std::string text;
    while(true){
        fgets(buffer, 1000000, fin);
        if(feof(fin)){
            break;
        }
        else{
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
                cGetImage newImage(this->mLog);
                text += "[[";
                text += newImage.getImage(fileName);
                text += "]]";
            }
            else{
                text += buffer;
            }
        }
    }
    fclose(fin);
    FILE* fout = fopen(out.c_str(), "w+");
    fprintf(fout, "%s \n", text.c_str());
    fclose(fout);
}
