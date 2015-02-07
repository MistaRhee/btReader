#ifndef BTREADER_HPP
#define BTREADER_HPP

#include <cstdio>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <utility>
#include <SDL2/SDL.h>
#include "xmlParser.h" //Nifty XML parsing library
#include "httpd.hpp" //For website retreival
#include "objects.hpp" //For SDL Objects

#define MAX_THREADS 5
#define FPS_CAP 1000/60

enum input_t{
    novelList,
    coverInfo,
    content
};

enum places_t{
    mainMenu,
    mainList,
    novelInfo,
    reader
};

class cWikiParser{
    public:
        std::string clean(const std::string); //Title of file to clean - Overwrites old file
        std::string getError();
        std::string generateRandomName(int len);
    private:
        std::string error;
        void setError(std::string);
}

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
        std::string getNovelDetails(std::string);
        void preComp();
        bool readDatabase();
        void createDatabase();
        void updateDatabase();
        void setError(std::string);
    private:
        std::map<std::string, std::string> novelDB; //Title, place to read from
        std::string error;
        std::vector<std::thread> processes;
        places_t whereAt;
        const std::string domain = "http://baka-tsuki.org/project/api.php?action=";
        const std::string novelList = "query&list=categorymembers&cmtitle=Category:Light_novel_(English)&cmlimit=500&format=xml";
        const std::string pageDetail = "parse&prop=wikitext|revid&format=xml&page="; //Add novel name after this (NAME_TITLE_ETC/CHAPTER_NAME)
        int currThreads;
        SDL_Window* mWindw;
        SDL_Renderer* mRenderer;
        std::vector<cTextBox> text;
        std::vector<cImage> images;


};

#endif //BTREADER_HPP
