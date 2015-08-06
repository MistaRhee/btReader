#include "btreader.hpp"

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

std::string cGetImage::generateRandomName(int length){       
    srand(time(NULL));     
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";       
    std::string rVal;      
    for(int i = 0, j = strlen(aCharacters); i < length; i++){      
        rVal += aCharacters[rand()%j];     
    }      
    return rVal;   
}

std::string cGetImage::getImage(const std::string fileName){
    if(fileName.size() > 0){
        std::string sauce = sanitize(fileName);
        try{
            printf("Grabbing image %s\n", sauce.c_str());
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
                tempFile = imageStore+generateRandomName(25);
                while(fileExists(tempFile)){ //Just to ensure no double ups in name
                    tempFile = "data/image/"+generateRandomName(25);
                }
                mDownload.download(imageSource, tempFile);
                return tempFile;
            }
        }
        catch(mException& e){
            printf("%s: [getImage.cpp] %s\n", currentDateTime().c_str(), e.what());
            return "system/images/notHere.jpg";
        }
    }
    else return "system/images/notHere.jpg";
}
