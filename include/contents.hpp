#ifndef CONTENTS_HPP
#define CONTENTS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <utility>
#include <vector>
#include <cstring>
#include <string>

/* Including the base class I'm deriving this from */
#include "objects.hpp" 

/* Including the XML parsing library */
#include "xmlParser.h"

namespace beatOff{ //Because it's a derived class and I would like to keep namespaces

    class cContent : public cObject{
        public:
            /* Setting functions */
            void setFont(std::string);
            void setFontSize(int);
            void setTextColour(SDL_Color);
            void setBackColour(SDL_Color);
            bool inOver(int, int); //x and y position of the mouse
            void inFocus(); //Turns on focus
            void offFocus(); //Turns off focus
            bool isInFocus();
            void use();
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
            bool inFocus;
            bool inUse;
    };
    
    class cMenu : public cContent{ //For the main menu (headers etc.)
        public:
            void render(SDL_Renderer*);
            void handleUserMouse(int, int, int, bool);
            std::string getSelected; //Returns the name of the object that is currently selected
        private:
            void moveSelection(int); //ds
            std::map<std::string, beatOff::cImage> images;
            std::map<std::string, beatOff::cButton> butons;
    };

    class cNovelList : public cContent{ //Config part of user Profile
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
        private:
            SDL_Texture* mTexture; //Texture which stores the stuff
            SDL_Rect sauceRect;
            bool loaded;
            std::string fileLoc;
            int selection;
    };
    
}
#endif //CONTENTS_HPP
