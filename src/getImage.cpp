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
    printf("Grabbing image %s\n", fileName.c_str());
    std::string imageInfo = imageQuery+fileName+"&";
    cHttpd mDownload;
    printf("Getting URL of actual file\n");
    std::string tempFile = "data/temp/"+generateRandomName(50);
    while(fileExists(tempFile)){ //Just to ensure no double ups in name
        tempFile = "data/temp/"+generateRandomName(50);
    }
    mDownload.download(imageInfo, tempFile.c_str());
    printf("Image info saved to %s\n", tempFile.c_str());
    XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
    std::string imageSource = mainNode.getChildNode("query").getChildNode("pages").getChildNode("page").getChildNode("imageinfo").getChildNode("ii").getAttribute("url");
    remove(tempFile.c_str());
    printf("Getting image \n");
    tempFile = "data/image/"+generateRandomName(25);
    while(fileExists(tempFile)){ //Just to ensure no double ups in name
        tempFile = "data/image/"+generateRandomName(25);
    }
    mDownload.download(imageSource, tempFile);
    printf("Image gotten\n");
    return tempFile;
}
