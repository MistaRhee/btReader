#include "btreader.hpp"

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

std::string cGetImage::getImage(const std::string fileName){
    std::string imageInfo = imageQuery+fileName;
    cHttpd mDownload;
    std::string tempFile = "data/temp/"+generateRandomName(25);
    while(fileExists(tempFile)){ //Just to ensure no double ups in name
        tempFile = "data/image/"+generateRandomName();
    }
    XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
    std::string imageSource = mainNode.getChildNode("query").getChildNode("pages").getChildNode("page").getChildNode("imageinfo").getChildNode("ii").getAttribute("url");
    remove(tempFile);
    tempFile = "data/image/"+generateRandomName();
    while(fileExists(tempFile)){ //Just to ensure no double ups in name
        tempFile = "data/image/"+generateRandomName();
    }
    mDownload.download(imageSource, tempFile);
    return tempFile;
}
