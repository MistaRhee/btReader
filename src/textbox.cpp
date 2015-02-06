#include "objects.hpp"

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

inline int mDiv(int x, int y){
    int returnVal = x/y;
    if (returnVal == 0){
        returnVal = 1;
    }
    return returnVal;
}

namespace beatOff{

    cTextBox::cTextBox(){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            printf("Error! SDL couldn't initialise. SDL Error: %s \n", SDL_GetError());
        } else if(!TTF_WasInit() && TTF_Init() == -1){
            printf("Error! TTF couldn't initialise. TTF Error: %s \n", TTF_GetError());
        } else{
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
    }

    cTextBox::cTextBox(std::string inText, std::string fontLoc, int inX, int inY, int inW, int inSize){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            printf("Error! SDL couldn't initialise. SDL Error: %s \n", SDL_GetError());
        } else if(!TTF_WasInit() && TTF_Init() == -1){
            printf("Error! TTF couldn't initialise. TTF Error: %s \n", TTF_GetError());
        } else{
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
        }
    }

    cTextBox::~cTextBox() {}

    void cTextBox::setText(std::string inText){
        text = inText;
    }

    void cTextBox::setTextSize(int inSize){
        textSize = inSize;
    }

    void cTextBox::showBox(){
        drawBox = 1;
    }

    void cTextBox::hideBox(){
        drawBox = 0;
    }

    void cTextBox::setFont(std::string inLoc){
        font = inLoc;
    }

    void cTextBox::setTextCol(int inR, int inG, int inB, int inA){
        textR = inR;
        textG = inG;
        textB = inB;
        textA = inA;
    }

    void cTextBox::setBoxCol(int inR, int inG, int inB, int inA){
        boxR = inR;
        boxG = inG;
        boxB = inB;
        boxA = inA;
    }

    void cTextBox::render(SDL_Renderer* mRenderer){
        if(!fileExists(font)){
            printf("cTextBox Error! Font does not exist! \n");
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "A fatal error has occurred! There is no such font! Please ensure that the file exists and game is installed correctly. If so, please contact the developers for more assistance.", NULL);
        }
        else{
            TTF_Font* mFont = TTF_OpenFont(font.c_str(), textSize);
            int tempW, multiplier = 1, lineSkip = TTF_FontLineSkip(mFont);
            int space = -1;
            std::string temp;
            std::vector<std::string> lines;
            SDL_Rect dRect;
            SDL_Texture* mTexture = NULL;
            SDL_Surface* mSurface = NULL;
            SDL_Colour mColour = {textR, textG, textB, textA};
            for(int i = 0, j = text.size(); i < j; i++){
                temp += text[i];
                if(text[i] == ' '){
                    space = i;
                }
                TTF_SizeText(mFont, temp.c_str(), &tempW, &h);
                if(tempW > w){
                    if(space < 0){
                        temp.pop_back();
                        i--;
                        lines.push_back(temp);
                        temp.clear();
                    }
                    else{
                        for(int k = 0, l = i-space; k < l; k++){
                            temp.pop_back();
                            i--;
                        }
                        lines.push_back(temp);
                        temp.clear();
                        space = -1;
                    }
                }
            }
            if(temp.size() > 0){
                lines.push_back(temp);
            }
            if(drawBox){
                h *= multiplier;
                dRect.x = x;
                dRect.y = y;
                dRect.w = w;
                dRect.h = h;
                SDL_SetRenderDrawColor(mRenderer, boxR, boxG, boxB, boxA);
                SDL_RenderFillRect(mRenderer, &dRect);
            }
            h = TTF_FontHeight(mFont);
            for(int i = 0, j = lines.size(); i < j; i++){
                TTF_SizeText(mFont, lines[i].c_str(), &w, &h);
                dRect.h = h;
                dRect.w = w;
                dRect.x = x;
                dRect.y = y+(i*lineSkip);
                mSurface = TTF_RenderText_Solid(mFont, lines[i].c_str(), mColour);
                mTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
                SDL_RenderCopy(mRenderer, mTexture, NULL, &dRect);
                SDL_FreeSurface(mSurface);
                SDL_DestroyTexture(mTexture);
            }
        }
    }
    
}
