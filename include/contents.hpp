#ifndef CONTENTS_HPP
#define CONTENTS_HPP

#include <algorithm>
#include <cstring>
#include <exception>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <utility>
#include <vector>

/* Including the base class I'm deriving this from */
#include "logger.hpp"
#include "objects.hpp" 
#include "types.hpp"

/* Including the XML parsing library */
#include "pugixml.hpp"

namespace beatOff{ //Because it's a derived class and I would like to keep namespaces

    class cContent : public cObject{
        public:
            /* Setting functions */
            cContent(); //To set the state to working so it doesn't immediately break -_-
            virtual ~cContent(){}

            void setFont(std::string);
            void setFontSize(int);
            void setTextColour(SDL_Color);
            void setBackColour(SDL_Color);
            virtual void handleUserMouse(int, int, int, bool) {}
            virtual void handleUserScroll(int, int) {}
            virtual void handleUserKeyboard(std::string, bool, unsigned int) {} //Name of the key, is pressed and the bitmask for modifiers

            virtual void reset() {this->state = working;} //Resets the state

            state_t state; //GLOBALLY ACCESSABLE STATE. YES PLEASE!
            std::string err; //Set if the state is stuck/broken. Indicates what needs to be logged with mLog->log()
        protected:
//            bool changed, created; //Not sure why these are here....
            std::string fontLoc;
            int fontSize;
            SDL_Color textColour;
            SDL_Color backColour;
            __logger::cLogger* mLog;
    };
    
    class cMenu : public cContent{ //For the main menu (headers etc.)
        public:
            cMenu();
            cMenu(__logger::cLogger*);
            ~cMenu() {}
            void render(SDL_Renderer*);
            void handleUserMouse(int, int, int, bool);
            void handleUserKeyboard(std::string, bool, unsigned int);
            void handleUserScroll(int, int);
            places_t getSelected(); //Returns the locationID of where to go next
            void addImage(std::string name, std::string sauce, int x, int y, int h, int w);
            void changeImage(std::string name, std::string sauce); //Switches the contents of an image
            void addButton(std::string name, std::string text, std::string font, int size, int x, int y, int h, int w);
            void selectButton(std::string name);
            std::mutex accessible; //TODO: Remove this hacky PoS after DLList is properly impelemnted
        private:
            /* TODO: Use polymorphism to make this nicer (ceebs ATM because I just want to get
             * something out)
             */
            std::map<std::string, beatOff::cImage> images;
            std::map<std::string, beatOff::cButton> buttons;
            bool mouseMoved;

            places_t next; //The location that has been selected
    };

    class cNovelList : public cContent{ //Config part of user Profile
        public:
            /* Init and starting defines */

            cNovelList();
            cNovelList(__logger::cLogger*);
            ~cNovelList();
            void setRect(SDL_Rect);

            /* "User" interactions */
            void addNovel(std::string in, int fontSize, std::string fontLoc = "system/fonts/default.ttf"); //Default fontloc is pretty fake
            void moveSelection(int); //ds
            std::string getSelected();

            /* Event handling */
            void handleUserMouse(int, int, int, bool);
            void handleUserScroll(int, int);
            void handleUserKeyboard(std::string, bool, unsigned int);

            /* Rendering to stuffs */
            void clear();
            void render(SDL_Renderer*);

        private:
            /* Internal functions */
            void genTexture(SDL_Renderer*);
            void move(int, int); //Overriding the cObject move function with one that moves the sauceRect instead of the dRect

            /* Settings constants */
            int novelHeight;
            std::string configLoc;
            int selected;

            /* Rendering objects */
            SDL_Texture* mTexture;
            SDL_Rect sRect;

            /* Storage */
            std::map<std::string, cButton> mNovels;
            std::vector<std::string> novelNames;

            /* Flags flags fags... */
            bool textureGen;
            bool freeScroll;
            bool inverted;
            int fsX; //The starting point of the free scroll
            int fsY; //The ending point of the free scroll
    };

    class cNovelDetails : public cContent{
        public:
            cNovelDetails();
            cNovelDetails(__logger::cLogger*);
            ~cNovelDetails() {}
            void setRect(SDL_Rect); //Only care about the rectangle's xPos and yPos and width
            void openNovel(std::string, SDL_Renderer*, std::map<std::string, std::multimap<std::string, std::map<std::string, std::string> > >&); //Opens up a novel from XML and then renders it to a texture
            void render(SDL_Renderer*); //Draws a section of the texture
            std::string getSelected();
            std::string getChapName();
            void handleUserMouse(int, int, int, bool);
            void handleUserScroll(int, int);
            void handleUserKeyboard(std::string, bool, unsigned int);
        private:
            void move(int, int); //Moves the image around (dx, dy). Should only require to move dy (scrolling)
            void moveSelection(int); //Moves selection
            void genTexture(SDL_Renderer*); //Regenerates texture (REQUIRES openNovel TO BE CALLED FIRST)

            std::vector<std::pair<cObject*, std::pair<std::string, unsigned int> > > contents; //Stores polymorphed objects and the cumulative height that that object contains (Bottom height of the object)
            SDL_Texture* mTexture; //Texture which stores the stuff
            SDL_Rect sRect;
            bool freeScroll;
            bool loaded;
            bool highlighted; //Flag for if mouse has focused on the buttons
            bool textureGen;
            int selection;

            int fsX, fsY; //Free scroll starting position
    };
    
}
#endif //CONTENTS_HPP
