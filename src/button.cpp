#include "objects.hpp"

namespace beatOff{

    cButton::cButton(){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string mError = currentDateTime() + ": ";
            mError += "[button.cpp] - SDL couldn't initialise (SDL Error: ";
            mError += SDL_GetError();
            mError += ")";
            setError(mError);
            printf("%s \n", mError.c_str());
        } 
        else if(!TTF_WasInit() && TTF_Init() == -1){
            std::string mError = currentDateTime() + ": ";
            mError += "[button.cpp] - TTF couldn't initialise (TTF Error: ";
            mError += SDL_GetError();
            mError += ")";
            setError(mError);
            printf("%s \n", mError.c_str());
        } 
        else{
            textR = 0;
            textG = 0;
            textB = 0;
            boxR = 255;
            boxG = 255;
            boxB = 255;
            textA = 255;
            boxA = 255;
            drawBox = 1;
            centered = 0;
        }
        selected = 0;
        showBox();
    }

    cButton::cButton(std::string inText, std::string fontLoc, int inX, int inY, int inH, int inW, int inSize){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string mError = currentDateTime() + ": ";
            mError += "[button.cpp] - SDL couldn't initialise (SDL Error: ";
            mError += SDL_GetError();
            mError += ")";
            setError(mError);
            printf("%s \n", mError.c_str());
        } 
        else if(!TTF_WasInit() && TTF_Init() == -1){
            std::string mError = currentDateTime() + ": ";
            mError += "[button.cpp] - SDL couldn't initialise (SDL Error: ";
            mError += SDL_GetError();
            mError += ")";
            setError(mError);
        } 
        else{
            textR = 0;
            textG = 0;
            textB = 0;
            boxR = 255;
            boxG = 255;
            boxB = 255;
            textA = 255;
            boxA = 255;
            drawBox = 1;
            centered = 0;
            setPos(inX, inY);
            setText(inText);
            setSize(inH, inW);
            setFont(fontLoc);
            setTextSize(inSize);
        }
        selected = 0;
        showBox();
    }
    void cButton::select(){
        if(!selected){
            setTextCol(255-textR, 255-textG, 255-textB, 255-textA);
            setBoxCol(255-boxR, 255-boxG, 255-boxB, 255-boxA);
            selected = 1;
        }
    }

    void cButton::deselect(){
        if(selected){
            setTextCol(255-textR, 255-textG, 255-textB, 255-textA);
            setBoxCol(255-boxR, 255-boxG, 255-boxB, 255-boxA);
            selected = 0;
        }
    }

}
