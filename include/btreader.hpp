#ifndef BTREADER_HPP
#define BTREADER_HPP

/*Notes to self (stuff I need to do later):
 *  - Reader
 *  - Multithreading (MUST DO)
 *  - Have the program also recognise parts of the main GUI as either buttons
 *  or images instead of only buttons for two and images for two
 *  - Have a file called updates or something, which allows the user to find
 *  out what happened recently (so basically a change log which will be an
 *  option)
 *  - Can download an external site translation onto user HD and open that HTML
 *  page with their default browser
 *  ************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif
#if __unix__
#include <dirent.h>
#endif
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
#include "xmlParser.h" //Nifty XML parsing library - It's depricated, however due to the small sized XML, I'm sticking with this.
#include "httpd.hpp" //For website retreival
#include "objects.hpp" //For SDL Objects
#include "reader.hpp" //For the reader... (I might just use an external project if I don't have the time

#define MAX_THREADS 5
#define FPS_CAP 1000/60

enum places_t{
    settings,
    showNovels,
    novelDetails,
    reader,
    dlList
    //updates (maybe implementing later)
};

class cGetImage{
    public:
        std::string getImage(const std::string); //File name, returns file saved location
        std::string generateRandomName(int);
    private:
        const std::string imageQuery = "http://www.baka-tsuki.org/project/api.php?action=query&prop=imageinfo&iiprop=url&format=xml&titles=";
        const std::string imageStore = "data/images/";
};

class cWikiParser{
    public:
        void cleanNovel(const std::string, const std::string); //Title of file to clean, title of location to save to
        void cleanChapter(const std::string, const std::string); //Title of the file to clean, title of the location to save to
        std::string getError();
        std::string generateRandomName(int len);
    private:
        std::string error;
        void setError(std::string);
};

class cMain{
    public:
        cMain();
        ~cMain();
        bool checkDependencies();
        void preComp();
        bool hasNew(const std::string); //Request title (checks revIds)
        bool run();
//        void update();
//        void processEvents();
        void render();
        void getObjects();
        std::string getError();
        std::string convTitle(std::string);
        std::string generateRandomName(int len);
        std::pair<std::string, std::string> getNovelDetails(std::string);
        bool readDatabase();
        void createDatabase();
        void updateDatabase();
        void replaceDatabase();
        void setError(std::string);
    private:
        const std::string tempLoc = "data/temp/";
        const std::string domain = "http://baka-tsuki.org/project/api.php?action=";
        const std::string novelList = "query&list=categorymembers&cmtitle=Category:Light_novel_(English)&cmlimit=500&format=xml";
        const std::string pageDetail = "parse&prop=wikitext|revid&format=xml&page="; //Add novel name after this (NAME_TITLE_ETC/CHAPTER_NAME)
        const std::string revID = "query&format=xml&prop=revisions&revlimit=1&titles="; //Add titles after this
        const std::string systemLoc = "system/";
        const std::string systemImagesLoc = "system/images/";
        places_t whereAt;
        int currThreads;
        int startRunTime;
        std::map<std::string, std::pair<std::string, std::string> > novelDB; //Title, place to read from
        std::string error;
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        std::map<std::string, beatOff::cImage> images;
        std::map<std::string, beatOff::cButton> buttons;
        std::map<std::string, std::string> fonts;
        std::map<std::string, SDL_Color> colours;
        beatOff::cNovelList* mNovelList;
//        beatOff::cReader* mReader;
//        beatOff::cNovelDetails* mNovelDetails;
};

#endif //BTREADER_HPP
