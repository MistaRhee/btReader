#include "btreader.hpp"

#define nameLength 16

void cWikiParser::clean(const std::string inFile){
    std::string outFile;
}

std::string cWikiParser::getError(){
    return error;
}

std::string cWikiParser::setError(std::string inError){
    error = inError;
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
