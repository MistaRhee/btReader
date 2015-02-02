#ifndef BTREADER_HPP
#define BTREADER_HPP

#include <cstdio>
#include <vector>
#include <queue>
#include <thread>
#include <pugixml/pugixml.hpp>
#include "httpd.hpp" //For website retrieval

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
        void preComp();
        void readDatabase();
        void createDatabase();
    private:
        std::map<std::string, std::string> pageDatabase; //Title, place to read from
        std::string errorCode;
        std::vector<std::thread> processes;
        places_t whereAt;

};

class cCrawler{
    public:
        cCrawler();
        bool run(const std::string, const std::string); //URL, title
        std::string getError();
    private:
        std::string URL;
        std::string dbTitle;
        std::string error();
        
};

class cClean{
    public:
        cClean(input_t, const std::string);
        void clean();
        void openFile();
    private:
        FILE* input;
        std::string loc;
        input_t type;
};

#endif //CRAWLER_HPP
