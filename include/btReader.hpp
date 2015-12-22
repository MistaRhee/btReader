#ifndef BTREADER_HPP
#define BTREADER_HPP

#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <exception>
#include <map>
#include <queue>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <utility>
#include <vector>

#include "contents.hpp" //For content objects
#include "httpd.hpp" //For website retreival
#include "keyMap.hpp" //For keymap class
#include "logger.hpp" //For logging ease
#include "mException.h" //For exception handling
#include "objects.hpp" //For SDL Objects
#include "pugixml.hpp" //Nifty XML parsing library - It's depricated, however due to the small sized XML, I'm sticking with this.
#include "types.hpp" //To hold global types

#define MAX_THREADS 5
#define FPS_CAP 1000/60

class cWebOut{
    public:
        cWebOut();
        cWebOut(__logger::cLogger*);
        ~cWebOut();
        void createPage(std::string sauce, std::string title);
        void displayPage();
        void cleanUp();

    private:
        __logger::cLogger* mLog;
        bool isReady;
        std::string tempLoc;
};

class cGetImage{
    public:
        cGetImage();
        cGetImage(__logger::cLogger*);
        ~cGetImage() {}
        std::string getImage(const std::string); //File name, returns file saved location

    private:

        __logger::cLogger* mLog;
        std::string generateRandomName(int len);
        std::string sanatize(const std::string);
        const std::string imageQuery = "http://www.baka-tsuki.org/project/api.php?action=query&prop=imageinfo&iiprop=url&format=xml&titles=";
        const std::string imageStore = "data/images/";
};

class cWikiParser{
    public:
        cWikiParser();
        cWikiParser(__logger::cLogger*);
        ~cWikiParser(){}
        void cleanNovel(const std::string, const std::string, const std::string); //Title of file to clean, title of the "exist" checking file, title of location to save to
        void cleanChapter(const std::string, const std::string); //Title of the file to clean, title of the location to save to
        std::string generateRandomName(int len);
    private:
        __logger::cLogger* mLog;
        std::string titleClean(const std::string);
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
        void setError();
        void close();
    private:
        /* Initialization functions */
        bool checkDependencies();
        void preComp();
        void getUserProfile();

        /* Sudoku function */
        void commitSudoku();

        /* Event handling */
        void handleUserKey(SDL_Keycode, bool, unsigned int); //key pressed, is it down, keyModifiers
        void handleUserMouse(int, int, int, bool); //x, y, mouse type, is down
        void handleUserScroll(int, int); //dx, dy

        /* The main loop */
        void processEvents();
        void update(); //Polls every single object and updates what is being shown etc. depending on their states
        /* NOTE: The main "overlord" class should not be rendering anything
         * itself, it should only be calling the "render" functions of all the
         * smaller subclasses of contents
         */
        void render();

        /* Database operations */
        bool hasNew(const std::string); //Request title (checks revIds)
        bool readDatabase();
        void createDatabase();
        void updateDatabase();
        void replaceDatabase();
        std::pair<std::string, std::string> getNovelDetails(std::string); //Returns name and location

        /* Logging needs */
        __logger::cLogger* mLog;

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
        bool error; //If there had been an error

        /* Graphics */
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        SDL_Rect contentLoc;

        /* Different screen contents stored here */
        std::map<places_t, beatOff::cContent*> mContents;

        /* Global storage */
        std::map<std::string, std::string> fonts;
        std::map<std::string, SDL_Color> colours; //Unsure if this is needed yet

        /* Background storage */
        std::map<std::string, std::pair<std::string, std::string> > novelDB; //Title: <location, revID>

        /* User stuff */
        beatOff::cKeyMap mKeys;

        /* Settings */
        std::map<std::string, std::multimap<std::string, std::multimap<std::string, std::string> > > config; //Group, tagName, attributeKey -> Attribute Value (wew)
        /* Config formatted as
         *  -> Subsection:  Key->value
         */

        /* Running check */
        bool running;
};

#endif //BTREADER_HPP
