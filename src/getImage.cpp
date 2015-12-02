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

bool cGetImage::isFromBT(std::string sauce){
    /* Should only have one http:// so I'll use this as base and if this isn't
     * good enough, I'll improve it */
    std::string locSauce = sauce;
    bool seenHTTP = 0, rVal = 1;
    for(int i = 0, j = sauce.size(); i < j; i++){
        if(locSauce[0] != 'h') locSauce.erase(locSauce.begin());
        else{
            if( locSauce[1] == 't' &&
                locSauce[2] == 't' &&
                locSauce[3] == 'p'
              ){
                if(!seenHTTP) seenHTTP = 1;
                else{
                    rVal = 0;
                    break;
                }
            }

        }
    }
    return rVal;
}

std::string cGetImage::getImage(const std::string fileName){
    if(fileName.size() > 0){
        std::string sauce = sanitize(fileName);
        try{
            if(!isFromBT(fileName)) throw("IMAGE URL NOT FROM BT PANIC!!!");
            printf("%s: [getImage.cpp] Grabbing image: %s\n", currentDateTime().c_str(),sauce.c_str());
            std::string imageInfo = imageQuery+sauce+"&";
            cHttpd mDownload;
            std::string tempFile = "data/temp/"+generateRandomName(50);
            while(fileExists(tempFile)){ //Just to ensure no double ups in name
                tempFile = "data/temp/"+generateRandomName(50);
            }
            mDownload.download(imageInfo, tempFile.c_str());
            pugi::xml_document doc;
            pugi::xml_parse_result res;

            if(res){
                pugi::xml_node mainNode = doc.child("api");
                std::string check = mainNode.child("query").child("pages").child("page").attribute("imagerepository").value();
                if(check.compare("local") != 0){
                    return "system/images/notHere.jpg";
                }
                else{
                    std::string imageSauce = mainNode.child("query").child("pages").child("page").child("imageinfo").child("ii").attribute("url").value();
                    remove(tempFile.c_str());
                /* Pull their naming system and create the folders needed to use this
                 * Guaranteed to be unique because they use this system
                 */
                std::string temp;
                tempFile = imageSauce;
                /* Remove fluff */
                for(int i = 0; i < imageSauce.size(); i++){
                    if(tempFile[i] == '/'){
                        if(temp == "image"){
                            tempFile.erase(0, temp.size()+1);
                            temp.clear();
                            break;
                        }
                        else{
                            tempFile.erase(0, temp.size()+1);
                            temp.clear();
                        }
                    }
                    else{
                        temp += imageSauce[i];
                    }
                }
                /* Grab subfolders, check their existence and create them if necessary */
                for(int i = 0; i < tempFile.size(); i++){
                    if(tempFile[i] == '/'){
                        if(!dirExists(temp)){
                            createFolder(temp);
                        }
                        temp += tempFile[i];
                    }
                    else{
                        temp += tempFile[i];
                    }
                }

                mDownload.download(imageSauce, tempFile);
                return tempFile;
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
            std::string mDT = currentDateTime();
            printf("%s: [getImage.cpp] %s\n", mDT.c_str(), e.what());
            return "system/images/notHere.jpg";
        }
    }
    else return "system/images/notHere.jpg";
}
