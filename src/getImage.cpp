#include "btReader.hpp"

#ifdef _WIN32
inline bool dirExists(const std::string& dirName) {
    DWORD ftyp = GetFileAttributesA(dirName.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif
#ifdef __unix__
inline bool dirExists(const std::string& dirName){
    DIR* myDir = NULL;
    myDir = opendir(dirName.c_str());
    if(myDir == NULL) return false;
    else return true;
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

cGetImage::cGetImage(){
    std::string logFile = "logs/";
    logFile += currentDateTime();
    logFile += " cGetImage.log";
    this->mLog = new __logger::cLogger(logFile);
}

cGetImage::cGetImage(__logger::cLogger* mLog){
    this->mLog = mLog;
}

std::string cGetImage::generateRandomName(int length){
    srand(time(NULL));
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string rVal;
    for(int i = 0, j = strlen(aCharacters); i < length; i++){
        rVal += aCharacters[rand()%j];
    }
    return rVal;
}

std::string sanitize(const std::string filename){
    std::string newString;
    for(int i = 0, j = filename.size(); i < j; i++){
        if(filename[i] == ' '){
            newString += "%20";
        }
        else if(filename[i] == '[' or filename[i] == ']'){
            /* Ignoring at the moment anyway */
        }
        else{
            newString += filename[i];
        }
    }
    return newString;
}

std::string cGetImage::getImage(const std::string fileName){
    std::string logInfo;
    if(fileName.size() > 0){
        std::string sauce = sanitize(fileName);
        try{
            logInfo = "[getImage.cpp] Info: Grabbing image ";
            logInfo += sauce;
            this->mLog->log(logInfo);
            logInfo.clear();

            std::string imageInfo = imageQuery+sauce+"&";
            cHttpd mDownload;
            std::string tempFile = "data/temp/"+generateRandomName(50);
            while(fileExists(tempFile)){ //Just to ensure no double ups in name
                tempFile = "data/temp/"+generateRandomName(50);
            }
            mDownload.download(imageInfo, tempFile.c_str());
            pugi::xml_document doc;
            pugi::xml_parse_result res = doc.load_file(tempFile.c_str());

            if(res){
                pugi::xml_node mainNode = doc.child("api");
                std::string check = mainNode.child("query").child("pages").child("page").attribute("imagerepository").value();
                if(check.compare("local") != 0){
                    this->mLog->log("[getImage.cpp] Warning: Invalid or missing image! Allocating to the notHere.jpg image!");
                    remove(tempFile.c_str());
                    return "system/images/notHere.jpg";
                }
                else{
                    std::string imageSauce = mainNode.child("query").child("pages").child("page").child("imageinfo").child("ii").attribute("url").value();
                    /* Pull their naming system and create the folders needed to use this
                     * Guaranteed to be unique because they use this system
                     */
                    std::string temp;
                    tempFile = imageSauce;
                    /* Remove fluff */
                    for(int i = 0; i < imageSauce.size(); i++){
                        if(imageSauce[i] == '/'){
                            if(temp == "image" or temp == "images"){
                                tempFile.erase(0, temp.size()+1);
                                printf("Break tempFile is: %s ImageSauce is: %s\n", tempFile.c_str(), imageSauce.c_str());
                                temp.clear();
                                break;
                            }
                            else{
                                tempFile.erase(0, temp.size()+1);
                                printf("tempFile is: %s temp is: %s\n", tempFile.c_str(), temp.c_str());
                                temp.clear();
                            }
                        }
                        else{
                            temp += imageSauce[i];
                        }
                    }
                    /* Grab subfolders, check their existence and create them if necessary */
                    temp.clear();
                    tempFile = std::string("data/images/") + tempFile;
                    for(int i = 0; i < tempFile.size(); i++){
                        if(tempFile[i] == '/'){
                            if(!dirExists(temp)){
                                printf("Creating folder %s \n", temp.c_str());
                                createFolder(temp);
                            }
                            temp += tempFile[i];
                        }
                        else{
                            temp += tempFile[i];
                        }
                    }
                    printf("TempFile: %s ImageSource: %s \n", tempFile.c_str(), imageSauce.c_str());

                    mDownload.download(imageSauce, tempFile);
                    return tempFile;
                }
            }
            else{
                /* XML Failed to load! */
                std::string e = "[cGetImage] Error: ";
                e += imageInfo;
                e += " could not be parsed. Description: ";
                e += res.description();
                remove(tempFile.c_str());
                throw(mException(e));
            }
        }
        catch(mException& e){
            printf("Caught? \n");
            this->mLog->log(e.what());
            return "system/images/notHere.jpg";
        }
    }
    else{
        printf("Size = 0 \n");
        this->mLog->log("[getImage.cpp] Warning: Invalid or missing image! Allocating to the notHere.jpg image!");
        return "system/images/notHere.jpg";
    }
}
