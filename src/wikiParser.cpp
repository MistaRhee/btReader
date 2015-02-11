#include "btreader.hpp"

#define TITLE 0
#define SYNOPSIS 1
#define INFO 2
#define VOLUMES 3
#define CHAPTERS 4

void cWikiParser::clean(const std::string inFile, const std::string outFile){
    FILE*fin = fopen(inFile.c_str(), "r");
    FILE*fout = fopen(outFile.c_str(), "w+");
    char buffer[4096];
    std::string currText;
    int stage = 0;
    XML
    while(!feof()){
        fgets(buffer, 4096, fin);
        for(int i = 0, j = strlen(buffer); i < j; i++){
            if(buffer[i] == ' '){
                switch(stage){
                    case TITLE:
                        asdf
                        break;
                    case SYNOPSIS:
                        asdf
                        break;
                    case INFO:
                        asdf
                        break;
                    case VOLUMES:
                        asdf
                        break;
                    case CHAPTERS:
                        asdf
                        break;
                    default:
                        printf("You really shouldn't be here.... \n");
                        break;
                }
                currText.clear();
            }
            else{
                currText += buffer[i];
            }
        }
    }
}

std::string cWikiParser::getError(){
    return error;
}

std::string cWikiParser::setError(std::string inError){
    error = inError;
}
