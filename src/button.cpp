#include "objects.hpp"

namespace beatOff{

    cButton::cButton(){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string e = "cTextBox Error - SDL couldn't initialise (SDL Error: " + SDL_GetError() + ")";
            setError(e);
            printf("%s \n", e.c_str());
        } 
        else if(!TTF_WasInit() && TTF_Init() == -1){
            std::string e = "cTextBox Error - TTF Couldn't initialise (TTF Error: " + TTF_GetError() + ")";
            setError(e);
            printf("%s \n", e.c_str());
        } 
        else{
            textR = 0;
            textG = 0;
            textB = 0;
            boxR = 0;
            boxG = 0;
            boxB = 0;
            textA = 255;
            boxA = 255;
            drawBox = 0;
        }
        showBox();
    }

    cButton::cButton(std::string inText, std::string fontLoc, int inX, int inY, int inW, int inSize){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string e = "cTextBox Error - SDL couldn't initialise (SDL Error: " + SDL_GetError() + ")";
            setError(e);
            printf("%s \n", e.c_str());
        } 
        else if(!TTF_WasInit() && TTF_Init() == -1){
            std::string e = "cTextBox Error - TTF Couldn't initialise (TTF Error: " + TTF_GetError() + ")";
            setError(e);
            printf("%s \n", e.c_str());
        } 
        else{
            textR = 0;
            textG = 0;
            textB = 0;
            boxR = 0;
            boxG = 0;
            boxB = 0;
            textA = 255;
            boxA = 255;
            drawBox = 0;
            setPos(inX, inY);
            setText(inText);
            setSize(0, inW);
            setFont(fontLoc);
            setTextSize(inSize);
            showBox();
        }
    }
    void cButton::select(){
        setBoxCol(255-boxR, 255-boxG, 255-boxB, 255-boxA);
    }

}
