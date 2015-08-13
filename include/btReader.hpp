#ifndef BTREADER_HPP
#define BTREADER_HPP

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
#include <exception>

#include "xmlParser.h" //Nifty XML parsing library - It's depricated, however due to the small sized XML, I'm sticking with this.
#include "httpd.hpp" //For website retreival
#include "objects.hpp" //For SDL Objects
#include "contents.hpp" //For content objects
#include "mException.h" //For exception handling
#include "keyMap.hpp" //For keymap class

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
        std::string generateRandomName(int len);
    private:
        std::string sanatize(const std::string);
        const std::string imageQuery = "http://www.baka-tsuki.org/project/api.php?action=query&prop=imageinfo&iiprop=url&format=xml&titles=";
        const std::string imageStore = "data/images/";
};

class cWikiParser{
    public:
        void cleanNovel(const std::string, const std::string, const std::string); //Title of file to clean, title of location to save to
        void cleanChapter(const std::string, const std::string); //Title of the file to clean, title of the location to save to
        std::string getError();
        std::string generateRandomName(int len);
    private:
        std::string error;
        void setError(std::string);
};

class cMain{
    public:
        /* Initializers and deconstructors */
        cMain();
        ~cMain();

        /* The "running" command */
        bool run();

        /* General 'helper' functions */
        std::string convTitle(std::string);
        std::string generateRandomName(int len);

        /* Error Handling */
        void setError(std::string);
        std::string getError();
    private:
        /* Initialization functions */
        bool checkDependencies();
        void preComp();
        void getObjects();
        void getUserProfile();

        /* Event handling */
        void handleUserKey(SDL_Keycode, bool, unsigned int); //key pressed, is it down, keyModifiers
        void handleUserMouse(int, int, int, bool); //x, y, mouse type, is down
        void handleUserScroll(int, int); //dx, dy

        /* The main loop */
//        void update();
        void processEvents();
        void render();

        /* Database operations */
        bool hasNew(const std::string); //Request title (checks revIds)
        bool readDatabase();
        void createDatabase();
        void updateDatabase();
        void replaceDatabase();
        std::pair<std::string, std::string> getNovelDetails(std::string); //Returns name and location

        /* URL constant strings */
        const std::string tempLoc = "data/temp/";
        const std::string domain = "http://baka-tsuki.org/project/api.php?action=";
        const std::string novelList = "query&list=categorymembers&cmtitle=Category:Light_novel_(English)&cmlimit=500&format=xml";
        const std::string pageDetail = "parse&prop=wikitext|revid|links&format=xml&page="; //Add novel name after this (NAME_TITLE_ETC/CHAPTER_NAME)
        const std::string revID = "query&format=xml&prop=revisions&revlimit=1&titles="; //Add titles after this

        /* System storage */
        const std::string systemLoc = "system/";
        const std::string systemImagesLoc = "system/images/";

        /* Main program control */
        places_t whereAt;
        int currThreads;
        int startRunTime;
        std::string error;

        /* Graphics */
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        SDL_Rect contentLoc;

        /* Storage */
        std::map<std::string, beatOff::cImage> images;
        std::map<std::string, beatOff::cButton> buttons;
        std::map<std::string, std::string> fonts;
        std::map<std::string, SDL_Color> colours;
        std::map<std::string, std::pair<std::string, std::string> > novelDB; //Title, place to read from

        /* Different Screens */
        beatOff::cNovelList mNovelList;
//        beatOff::cReader mReader;
//        beatOff::cNovelDetails mNovelDetails;

        /* User stuff */
        beatOff::cKeyMap mKeys;
        std::map<std::string, std::string> userProfile;
};

#endif //BTREADER_HPP
