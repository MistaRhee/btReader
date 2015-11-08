#include "btReader.hpp"

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
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
            XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
            std::string check = mainNode.getChildNode("query").getChildNode("pages").getChildNode("page").getAttribute("imagerepository");
            if(check.compare("local") != 0){
                return "system/images/notHere.jpg";
            }
            else{
                std::string imageSource = mainNode.getChildNode("query").getChildNode("pages").getChildNode("page").getChildNode("imageinfo").getChildNode("ii").getAttribute("url");
                remove(tempFile.c_str());
                
                /* Pull their naming system and create the folders needed to use this
                 * Guaranteed to be unique because they use this system
                 */
                std::string temp;
                tempFile = imageSource;
                /* Remove fluff */
                for(int i = 0; i < imageSource.size(); i++){
                    if(imageSource[i] == "/"){
                        if(temp == "image"){
                            imageSource.erase(0, temp.size()+1);
                            break;
                        }
                        else{
                            imageSource.erase(0, temp.size()+1);
                        }
                    }
                    else{
                        temp += imageSource[i];
                    }
                }

                /* Grab subfolders */

                mDownload.download(imageSource, tempFile);
                return tempFile;
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
