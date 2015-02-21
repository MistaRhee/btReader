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

namespace beatOff{

    class cObject{
        public:
            cObject();
            virtual void render(SDL_Renderer*) {}
            void setPos(int, int);
            void setSize(int, int);
            void move(int, int);
            void setError(std::string);
            std::string getError;
            bool hasError();
        protected:
            int x, y;
            int h, w;
            std::string error;
    };
    
    class cTextBox : public cObject{
        public:
            cTextBox(); //Initialise
            cTextBox(std::string, std::string, int, int, int, int); //Initialise while setting text, xPos, yPos, width, fontSize. Height is calculated within the render function
            ~cTextBox();
            void setText(std::string);
            void setTextSize(int);
            void showBox();
            void hideBox();
            void setFont(std::string); //Font must be set before using the following functions - canFit, render
            void setTextCol(int, int, int, int);
            void setBoxCol(int, int, int, int);
            bool canFit(int); //Given the height (the rest of the stuff is kept the same), checks if the text can actually fit within the height
            void render(SDL_Renderer*);
        protected:
            std::string text;
            std::string font;
            int textSize;
            int textR, textG, textB, textA;
            int boxR, boxG, boxB, boxA;
            bool drawBox;

    };

    class cImage : public cObject{
        public:
            cImage(); //Initialise
            cImage(std::string, int, int, int, int);//Initialise while setting location, xPos, yPos, height, width (-1 height and width to make image height)
            void setPicLoc(std::string);
            void render(SDL_Renderer*);
        private:
            std::string location;

    };

    class cButton : public cTextBox {
        public:
            cButton(); //Initialise
            cButton(std::string, std::string, int, int, int, int); //Initialise while setting text, xPos, yPos, width, fontSize. Height is calculated within the render function
            void select();
    };

}
#endif //OBJECTS_HPP
