#include "objects.hpp"

inline bool fileExists (const std::string& name){
    if(FILE *file = fopen(name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
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
            std::string mError = currentDateTime() + ": ";
            mError += "[textbox.cpp] - SDL couldn't initialise (SDL Error: ";
            mError += SDL_GetError();
            mError += ")";
            setError(mError);
            printf("%s \n", mError.c_str());
        } 
        else if(!TTF_WasInit() && TTF_Init() == -1){
            std::string mError = currentDateTime() + ": ";
            mError = "[textbox.cpp] - TTF Couldn't initialise (TTF Error: ";
            mError += TTF_GetError();
            mError += ")";
            setError(mError);
            printf("%s \n", mError.c_str());
        } 
        else{
            textSize = 12; //Defaulted to this ATM
            textR = 0;
            textG = 0;
            textB = 0;
            boxR = 0;
            boxG = 0;
            boxB = 0;
            textA = 255;
            boxA = 255;
            drawBox = 1;
            centered = 0;
        }
    }

    cTextBox::cTextBox(std::string inText, std::string fontLoc, int fontSize, int inX, int inY, int inW, int inH){
        if(!SDL_WasInit(SDL_INIT_EVERYTHING) && SDL_Init(SDL_INIT_EVERYTHING) < 0){
            std::string mError = currentDateTime() + ": ";
            mError += "[textbox.cpp] - SDL couldn't initialise (SDL Error: ";
            mError += SDL_GetError();
            mError += ")";
            setError(mError);
            printf("%s \n", mError.c_str());
        } 
        else if(!TTF_WasInit() && TTF_Init() == -1){
            std::string mError = currentDateTime() + ": ";
            mError += "[textbox.cpp] - TTF Couldn't initialise (TTF Error: ";
            mError += TTF_GetError();
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
            setPos(inX, inY);
            setText(inText);
            setSize(inH, inW);
            setFont(fontLoc);
            setTextSize(fontSize);
        }
    }

    bool cTextBox::isCentered(){
        return centered;
    }

    void cTextBox::centre(){
        centered = !(centered);
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

    void cTextBox::invert(){
        setTextCol(255-textR, 255-textG, 255-textB, 255-textA);
        setBoxCol(255-boxR, 255-boxG, 255-boxB, 255-boxA);
    }

    int cTextBox::wrappedHeight(){
        int renderedHeight = -1;
        if(!fileExists(font)){
            std::string mError = currentDateTime();
            mError += "[textbox.cpp] - Font doesn't exist (Font location = ";
            mError += font;
            mError += ")";
            printf("%s\n", mError.c_str());
            setWarning(mError); //Only warning the user (maybe they forgot to set the font before the test... IDK LOL!
        }
        else{
            int tempWidth, numLines = 0, space = -1;
            std::string temp;
            TTF_Font* mFont = TTF_OpenFont(font.c_str(), textSize);
            for(int i = 0; i < text.size(); i++){
                temp += temp[i];
                if(text[i] == ' '){
                    space = i;
                }
                TTF_SizeText(mFont, temp.c_str(), &tempWidth, &renderedHeight);
                if(tempWidth > w){
                    if(space < 0){
                        i--;
                        temp.clear();
                    }
                    else{
                        i = space;
                        space = -1;
                        temp.clear();
                    }
                    numLines ++;
                }
            }
            renderedHeight = (TTF_FontHeight(mFont)*numLines) + (TTF_FontLineSkip(mFont)* numLines);
            TTF_CloseFont(mFont);
        }
        return(renderedHeight);
    }

    bool cTextBox::canFit(int incomingHeight){
        return(wrappedHeight() < incomingHeight);
    }

    std::string cTextBox::getText(){
        return text;
    }

    void cTextBox::render(SDL_Renderer* mRenderer){
        if(!fileExists(font)){
            std::string mError = currentDateTime() + ": ";
            mError += "cTextBox Error - Font doesn't exist (Font location = ";
            mError += font;
            mError += ")";
            setError(mError); //Now its an error if you try to render without a font
            printf("%s\n", mError.c_str());
        }
        else{
            TTF_Font* mFont = TTF_OpenFont(font.c_str(), textSize);
            if(h > 0){ //If we have a set height
                int expected = wrappedHeight(), tempW;
                if(h > expected){
                    std::string mWarning = currentDateTime() + ": ";
                    mWarning = "[textbox.cpp] Inputted height is too small by ";
                    mWarning += std::to_string(h - wrappedHeight());
                    mWarning += " pixels! Prentending if the guideline height didn't exist";
                    setWarning(mWarning);
                    printf("%s \n", mWarning.c_str());
                }
                else{
                    y += (expected - h)/2;
                    TTF_SizeText(mFont, text.c_str(), &tempW, &expected);
                    if(w < tempW){
                        x += (tempW-w)/2;
                    }
                }
            }
            int tempW, multiplier = 1, lineSkip = TTF_FontLineSkip(mFont);
            int space = -1;
            std::string temp;
            std::vector<std::string> lines;
            SDL_Rect dRect;
            SDL_Texture* mTexture = NULL;
            SDL_Surface* mSurface = NULL;
            SDL_Colour mColour = {static_cast<Uint8>(textR), static_cast<Uint8>(textG), static_cast<Uint8>(textB), static_cast<Uint8>(textA)};
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
                TTF_SizeText(mFont, lines[i].c_str(), &tempW, &h);

                if(centered){
                    dRect.x = x + (w-tempW)/2; //It'll favour shifting towards the left by one pixel. w/e
                }
                else{
                    dRect.x = x;
                }
                dRect.y = y+(i*lineSkip);
                dRect.h = h;
                dRect.w = tempW;

                mSurface = TTF_RenderText_Blended(mFont, lines[i].c_str(), mColour);
                mTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
                SDL_RenderCopy(mRenderer, mTexture, NULL, &dRect);
                SDL_FreeSurface(mSurface);
                SDL_DestroyTexture(mTexture);
            }
            TTF_CloseFont(mFont);
        }
    }

    void cTextBox::renderWikiText(SDL_Renderer* mRenderer){
        /* Wiki Text rules:
         *  Rendering word by word instead of line by line
         *
         *  - tl;dr It's basically the same as normal rendering, just watch out
         *  for special things (the triple quotes etc..)
         *  '''WORD''' -> Bold (I think)
         *  ''WORD'' -> Italic (I think)
         *  '''''WORD''''' -> Bold+Italic (Guaranteed)
         *  =WORD= -> Heading (so like 2x size + bold?)
         *  
         *  To make life easier, I'll assume that making something bold doesn't
         *  increase the width (otherwise it'll become rather non-trivial to
         *  do)
         *
         *  TTF Styles:
         *      TTF_STYLE_BOLD | TTF_STYLE_ITALIC | TTF_STYLE_UNDERLINE |
         *      TTF_STYLE_STRIKETHROUGH | TTF_STYLE_NORMAL
         *
         *      TTF_SetFontStyle(mFont, BITMASK);
         */
        if(!fileExists(font)){
            std::string mError = currentDateTime() + ": ";
            mError += "cTextBox Error - Font doesn't exist (Font location = ";
            mError += font;
            mError += ")";
            setError(mError); //Now its an error if you try to render without a font
            printf("%s\n", mError.c_str());
        }
        else{
            std::vector<std::string> mWords;
            std::string tempStr;

            /* For safety */
            tempStr.clear();
            mWords.clear();

            for(int i = 0, j = text.size(); i < j; i++){
                /* Spltting text into word chunks */
            }
        }
    }

}
