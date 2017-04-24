#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#ifdef __unix__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#endif

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#endif

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
            cObject(int, int, int, int);//x, y, w, h
            virtual ~cObject() {}
            virtual void render(SDL_Renderer*) {}
            void setPos(int, int); //x, y
            void setSize(int, int); //w, h
            void move(int, int); //dx, dy
            void setError(std::string);
            void setWarning(std::string);
            bool hasWarning();
            bool isOver(int, int); //x and y position of the mouse
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
            /* Set H to be -1 for it to set h to be equal to wrappedHeight() */
            cTextBox(); //Initialise
            cTextBox(std::string contents, std::string fontLoc, int fontSize, int inX, int inY, int inW, int inH = -1); 
            virtual ~cTextBox();
            void setText(std::string);
            void setTextSize(int);
            void showBox();
            void hideBox();
            void setFont(std::string); //Font must be set before using the following functions - canFit, render
            void setTextCol(int, int, int, int); //RGBA
            void setBoxCol(int, int, int, int); //RGBA
            int wrappedHeight(); //Returns the expected height of the rendering
            int getFontHeight(); //Returns the height of the glyph
            bool canFit(int); //Given the height (the rest of the stuff is kept the same), checks if the text can actually fit within the height
            std::string getText();
            void render(SDL_Renderer*);
            void centre();
            void invert(); //Inverts text and box colour
            bool isCentered();
            void compact(); //Sets compacted
            bool isCompacted();
        protected:
            std::string text;
            std::string font;
            int textSize;
            int textR, textG, textB, textA;
            int boxR, boxG, boxB, boxA;
            bool drawBox;
            bool centered; //If set, this will make any non-width filling line (wrapped or unwrapped) centered.
            bool compacted; //If set, if the text can't fit, it will remove the end of the text and add ellipsis
            std::string compactedText; //To prevent recalculations. It will memorise what was done before
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

    class cWikiTextBox : public cTextBox{
        public:
            cWikiTextBox() : cTextBox() {}
            cWikiTextBox(std::string contents, std::string fontLoc, int fontSize, int inX, int inY, int inW, int inH = -1) : cWikiTextBox(contents, fontLoc, fontSize, inX, inY, inW,  inH) {}

            void render(SDL_Renderer*); //Overloading because reasons.
    };

    class cButton : public cTextBox {
        public:
            /* Shares the same height properties of textBox */
            cButton(); //Initialise
            cButton(std::string inText, std::string fontLoc, int inSize, int inX, int inY, int inW, int inH = -1);
            void select();
            void deselect();
        private:
            bool selected;
    };

}
#endif //OBJECTS_HPP
