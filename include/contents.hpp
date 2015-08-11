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
            void setFont(std::string);
            void setFontSize(int);
            void setTextColour(SDL_Color);
            void setBackColour(SDL_Color);

            /* Event Handling -> Keyboard ignored as it is handled in main
             * event */
            virtual void handleUserMouse(int, int, int, bool) {}
            virtual void handleUserScroll(int, int) {}
        protected:
            bool changed, created;
            std::string fontLoc;
            int fontSize;
            SDL_Color textColour;
            SDL_Color backColour;
    };

    class cNovelList : public cContent{ //Config part of user Profile
        public:
            cNovelList();
            void setRect(SDL_Rect);
            void addNovel(std::string, int);
            void moveSelection(int);
            std::string getSelected();
            void handleUserMouse(int, int, int, bool);
            void handleUserScroll(int, int);
            void render(SDL_Renderer*);
            void genTexture();
        private:
            SDL_Texture* mTexture;
            SDL_Rect sauceRect;
            std::string configLoc;
            std::vector<cTextBox> mNovels;
            int selected;
            int novelHeight;
    };
    
    class cReader : public cContent{
        public:
            cReader();
            void setRect(SDL_Rect); //Sets the dimensions of the rendered object (so the program won't get confused)
            void render(SDL_Renderer*);
            void getContent(std::string); //Gets the content from a file and then renders it to a texture
        private:
            SDL_Texture* mTexture;
            bool loaded;
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
