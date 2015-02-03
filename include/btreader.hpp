#ifndef BTREADER_HPP
#define BTREADER_HPP

#include <cstdio>
#include <vector>
#include <queue>
#include <thread>
#include "xmlParser.h" //Nifty XML parsing library
#include "httpd.hpp" //For website retreival

#define MAX_THREADS 5

enum input_t{
    novelList,
    coverInfo,
    content
};

enum places_t{
    mainMenu,
    novelList,
    novelInfo,
    reader
};

class cMain{
    public:
        cMain();
        bool checkExist(const std::string); //Request title
        bool run();
        void update();
        void processEvents();
        void render();
        std::string getError();
        std::string convTitle(std::string);
        void preComp();
        void readDatabase();
        void createDatabase();
        void updateDatabase();
        void setError(std::string);
    private:
        std::map<std::string, std::string> pageDatabase; //Title, place to read from
        std::string error;
        std::vector<std::thread> processes;
        places_t whereAt;
        const std::string domain = "http://baka-tsuki.org/project/api.php?action=";
        const std::string novelList = "query&list=categorymembers&cmtitle=Category:Light_novel_(English)&cmlimit=500&format=xml";
        const std::string pageDetail = "parse&prop=wikitext|revid&format=xml&page="; //Add novel name after this (NAME_TITLE_ETC/CHAPTER_NAME)
        int currThreads;

};

#endif //CRAWLER_HPP
