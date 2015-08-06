#ifndef OBJEcTS_HPP
#define OBJECTS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <utility>
#include <vector>
#include <cstring>
#include <string>

#include "mException.h"

namespace beatOff{

    class cObject{
        public:
            cObject();
            virtual void render(SDL_Renderer*) {}
            void setPos(int, int); //x, y
            void setSize(int, int); //h, w
            void move(int, int); //dx, dy
            void setError(std::string);
            void setWarning(std::string);
            bool hasWarning();
            std::string getWarning();
            std::string getError();
            bool hasError();
        protected:
            int x, y;
            int h, w;
            std::string error;
            std::string warning;
    };

    class cTextBox : public cObject{
        public:
            cTextBox(); //Initialise
            cTextBox(std::string contesnts, std::string fontLoc, int fontSize, int inX, int inY, int inW, int inH = -1);
            ~cTextBox();
            void setText(std::string);
            void setTextSize(int);
            void showBox();
            void hideBox();
            void setFont(std::string); //Font must be set before using the following functions - canFit, render
            void setTextCol(int, int, int, int); //RGBA
            void setBoxCol(int, int, int, int); //RGBA
            int wrappedHeight(); //Returns the expected height of the rendering
            bool canFit(int); //Given the height (the rest of the stuff is kept the same), checks if the text can actually fit within the height
            std::string getText();
            void render(SDL_Renderer*);
            void centre();
            bool isCentered();
        protected:
            std::string text;
            std::string font;
            int textSize;
            int textR, textG, textB, textA;
            int boxR, boxG, boxB, boxA;
            bool drawBox;
            bool centered; //If set, this will make any non-width filling line (wrapped or unwrapped) centered.

    };

    class cImage : public cObject{
        /* To leave the image at its original size, set height and width to -1 */
        public:
            cImage(); //Initialise
            cImage(std::string inLoc, int inx, int iny, int inh = -1, int inw = -1);
            void setPicLoc(std::string);
            void render(SDL_Renderer*);
            std::pair<int, int> getSize(); //Will only work if location is set
        private:
            std::string location;

    };

    class cButton : public cTextBox {
        public:
            cButton(); //Initialise
            cButton(std::string inText, std::string fontLoc, int inX, int inY, int inW, int inSize);
            void select();
            void deselect();
        private:
            bool selected;
    };

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
            cNovelList(std::string config = "system/listDisp.xml");
            cNovelList(SDL_Rect* inRect, std::string config = "system/listDisp.xml");
            void addNovel(std::string);
            void moveSelection(int);
            std::string getSelected();
            void render(SDL_Renderer*);

        private:
            std::string configLoc;
            std::vector<cTextBox> mNovels;
            int selected;
    };
    
    class cReader : public cContent{
        public:
            cReader();
            cReader(SDL_Rect*); //Sets the dimensions of the rendered object (so the program won't get confused)
            void render(SDL_Renderer*);
            void getContent(std::string); //Gets the content from a file and then renders it to a texture
        private:
            SDL_Texture* mTexture;
            bool loaded;
    };

    class cNovelDetails : public cContent{
        public:
            cNovelDetails();
            cNovelDetails(SDL_Rect*); //Only care about the rectangle's xPos and yPos and width
            void openNovel(std::string, SDL_Renderer*); //Opens up a novel from XML and then renders it to a texture
            void render(SDL_Renderer*); //Draws a section of the texture
            void move(int, int); //Moves the image around (dx, dy). Should only require to move dy (scrolling)
            std::string getSelected();
        private:
            SDL_Texture* mTexture; //Texture which stores the stuff
            bool loaded;
            std::string fileLoc;
            int selection;
    };
    
}
#endif //OBJECTS_HPP
