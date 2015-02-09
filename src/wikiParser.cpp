#include "btreader.hpp"

void cWikiParser::clean(const std::string inFile, const std::string outFile){
    FILE*fin = fopen(inFile.c_str(), "r");
    FILE*fout = fopen(outFile.c_str(), "w+");
}

std::string cWikiParser::getError(){
    return error;
}

std::string cWikiParser::setError(std::string inError){
    error = inError;
}
