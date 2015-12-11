#ifndef CONTENTS_HPP
#define CONTENTS_HPP

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
#include "objects.hpp" 
#include "types.hpp"

/* Including the XML parsing library */
#include "pugixml.hpp"

struct contentException : public std::exception{
    const places_t* location() const throw(){
        return this->loc;
    }
    const unsigned int* what() const throw(){
        return this->what;
    }
    contentException(places_t* place, unsigned char what){
        this->loc = place;
        this->mask = what;
    }
    places_t loc;
    unsigned char mask; //A bitmask for the state. Each content type has a unique mask definition
};

namespace beatOff{ //Because it's a derived class and I would like to keep namespaces

    class cContent : public cObject{
        public:
            /* Setting functions */
            void setFont(std::string);
            void setFontSize(int);
            void setTextColour(SDL_Color);
            void setBackColour(SDL_Color);
            bool isOver(int, int); //x and y position of the mouse
            void inFocus(); //Turns on focus
            void offFocus(); //Turns off focus
            bool isInFocus();
            void inUse();
            void hide();
            bool isInUse();
            /* Event Handling */
            virtual void handleUserMouse(int, int, int, bool) {}
            virtual void handleUserScroll(int, int) {}
            virtual void handleUserKeyboard(std::string, bool, unsigned int) {} //Name of the key, is pressed and the bitmask for modifiers
        protected:
//            bool changed, created; //Not sure why these are here....
            std::string fontLoc;
            int fontSize;
            SDL_Color textColour;
            SDL_Color backColour;
            bool focus;
            bool use;
    };
    
    class cMenu : public cContent{ //For the main menu (headers etc.)
        public:
            void render(SDL_Renderer*);
            void handleUserMouse(int, int, int, bool);
            void handleUserKeyboard(std::string, bool, unsigned int);
            std::string getSelected; //Returns the name of the object that is currently selected
        private:
            void moveSelection(int); //ds
            std::map<std::string, beatOff::cImage> images;
            std::map<std::string, beatOff::cButton> butons;
    };

    class cNovelList : public cContent{ //Config part of user Profile
        /* Content return bitmask:
         * |7|6|5|4|3|2|1|0|
         * |.|.|.|.|.|.|.|g|
         *
         * g = Go (i.e. something was selected)
         * . = Unassigned
         */
        public:
            /* Init and starting defines */
            cNovelList();
            void setRect(SDL_Rect);

            /* "User" interactions */
            void addNovel(std::string, int);
            void moveSelection(int); //ds
            std::string getSelected();

            /* Event handling */
            void handleUserMouse(int, int, int, bool);
            void handleUserScroll(int, int);
            void handleUserKeyboard(std::string, bool, unsigned int);

            /* Rendering to stuffs */
            void render(SDL_Renderer*);
        private:
            /* Internal functions */
            void genTexture(SDL_Renderer*);

            /* Settings constants */
            int novelHeight;
            std::string configLoc;
            int selected;

            /* Rendering objects */
            SDL_Texture* mTexture;
            SDL_Rect sauceRect;

            /* Storage */
            std::vector<cTextBox> mNovels;

            /* Flags flags fags... */
            bool textureGen;
            bool mouseMoved;
    };

    class cNovelDetails : public cContent{
        public:
            cNovelDetails();
            void setRect(SDL_Rect); //Only care about the rectangle's xPos and yPos and width
            void openNovel(std::string, SDL_Renderer*, std::string); //Opens up a novel from XML and then renders it to a texture
            void render(SDL_Renderer*); //Draws a section of the texture
            void move(int, int); //Moves the image around (dx, dy). Should only require to move dy (scrolling)
            std::string getSelected();
            void handleUserMouse(int, int, int, bool);
            void handleUserScroll(int, int);
            void handleUserKeyboard(std::string, bool, unsigned int);
        private:
            SDL_Texture* mTexture; //Texture which stores the stuff
            SDL_Rect sauceRect;
            bool loaded;
            std::string fileLoc;
            int selection;
    };
    
}
#endif //CONTENTS_HPP
