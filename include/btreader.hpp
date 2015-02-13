#ifndef BTREADER_HPP
#define BTREADER_HPP

/*Notes to self (stuff I need to do later):
 *  - Image Downloader (Debugging now)
 *  - WikiParser (Debugging now)
 *  - Reader
 *  - Check validity of the pages (maybe do with the Wiki Parser - I'm only not
 *  doing it because it will make the code look bad)
 *  - Multithreading (MUST DO)
 *  - Can download an external site translation onto user HD and open that HTML
 *  page with their default browser
 *  - Have a file called updates or something, which allows the user to find
 *  out what happened recently (so basically a change log which will be an
 *  option)
 *  ************************************************************************/



#include <cstdio>
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

class cGetImage{
    public:
        std::string getImage(const std::string); //File name, returns file saved location
        std::string generateRandomName(int);
    private:
        const std::string imageQuery = "http://www.baka-tsuki.org/project/api.php?action=query&prop=imageinfo&iiprop=url&titles=";
        const std::string imageStore = "data/images/"
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
        bool hasNew(const std::string); //Request title (checks revIds)
        bool run();
        void update();
        void processEvents();
        void render();
        std::string getError();
        std::string convTitle(std::string);
        std::string generateRandomName(int len);
        std::pair<std::string, std::string> getNovelDetails(std::string);
        void preComp();
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
        const std::string revID = "query&prop=revisions&revlimit=1&titles="; //Add titles after this
        places_t whereAt;
        int currThreads;
        std::map<std::string, std::pair<std::string, std::string> > novelDB; //Title, place to read from
        std::string error;
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        std::vector<cTextBox> text;
        std::vector<cImage> images;
};

#endif //BTREADER_HPP
