#ifndef WIKIPARSER_HPP
#define WIKIPARSER_HPP

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <utility>
#include <ctime>
#include <SDL2/SDL.h>
#include <string>
#include <cstring>
#include <exception>


struct Links{
    bool internal;
    bool available;
    bool page;          //true for page, false for file/image
    std::string link;
    std::string text;
};

struct WikiText{
    char type;
    int level;
    std::string text;
};


class cWikiParser{
    public:
        void openAP(const std::string, const std::string);       //send data returned from api query toi be opened. 
        void close();                       //cklears registry of data.
        int nquery(const std::string);      //get count of a ceratin element
        std::string query(const std::string);                   //query for the data in the withint the element with their text = string.
        std::string query(const std::string, const int);//query the ith element under the text = string.
    private:
        std::string space2Undersc(const std::string);   //cleans out strings to remove spaces and replace with underscore
        std::string textCleaner(std::string);   //cleans texts of links and images replacing with UID as well as marks up bold and italic fonts.
        int externalLink(std::string);         //check and fill in the external links, I might put it all into one, but putting the splitter into link, but the splitter also filters out non-links.
        int internalLink(std::string);

        //storage of information for later use
        std::vector<WikiText> database;
        std::vector<Links> linkDB;
        std::map<std::string, std::pair<int, int>> searchDB;
	    std::map<std::string, int> availMap;        //map with the string and availability

};

#endif//WIKIPARSER_HPP
