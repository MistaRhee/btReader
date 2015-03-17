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
            cTextBox(std::string, std::string, int, int, int, int, int); //Initialise while setting text, xPos, yPos, width, height, fontSize. (Height can be left as -1 if you want it to be calculated in the actual function, otherwise, it will be the height of the bounding box.
            ~cTextBox();
            void setText(std::string);
            void setTextSize(int);
            void showBox();
            void hideBox();
            void setFont(std::string); //Font must be set before using the following functions - canFit, render
            void setTextCol(int, int, int, int);
            void setBoxCol(int, int, int, int);
            int wrappedHeight(); //Returns the expected height of the rendering
            bool canFit(int); //Given the height (the rest of the stuff is kept the same), checks if the text can actually fit within the height
            std::string getText();
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
        /* To leave the image at its original size, set height and width to -1 */
        public:
            cImage(); //Initialise
            cImage(std::string, int, int, int, int);//Initialise while setting location, xPos, yPos, height, width (-1 height and width to make image height)
            void setPicLoc(std::string);
            void render(SDL_Renderer*);
            std::pair<int, int> getSize(); //Will only work if location is set
        private:
            std::string location;

    };

    class cButton : public cTextBox {
        public:
            cButton(); //Initialise
            cButton(std::string, std::string, int, int, int, int); //Initialise while setting text, xPos, yPos, width, fontSize. Height is calculated within the render function
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
        protected:
            bool changed, created;
            std::string fontLoc;
            int fontSize;
            SDL_Color textColour;
            SDL_Color backColour;
    };

    class cNovelList : public cContent{
        public:
            cNovelList(SDL_Rect*); //Sets the position start and the rendered size of the object
            void addNovel(std::string);
            void moveSelection(int);
            std::string getSelected();
            void render(SDL_Renderer*);
        private:
            const int novelHeight = 50; //Subject to change
            std::vector<cTextBox> mNovels;
            int selected;
    };
    
    class cReader : public cContent{
        public:
            cReader(SDL_Rect*); //Sets the dimensions of the rendered object (so the program won't get confused)
            void render(SDL_Renderer*);
            void getContent(std::string); //Gets the content from a file and then renders it to a texture
        private:
            SDL_Texture* mTexture;
            bool loaded;
    };
/*
    class cNovelDetails : public cContent{
        public:
            cNovelDetails(SDL_Rect*);
            void render(SDL_Renderer*);
            void setFile(std::string);
            void update(SDL_Event&);
            bool selected();
        private:
            std::string fileLoc;
    }
*/    
}
#endif //OBJECTS_HPP
