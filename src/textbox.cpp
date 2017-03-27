#include "objects.hpp"

#ifdef __unix__
inline bool fileExists (const std::string& name){
    if(FILE *file = fopen(name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
        return false;
    }
}
#endif

#ifdef _WIN32
inline bool fileExists (const std::string& name){
    FILE* file = NULL;
    if(!fopen_s(&file, name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
        return false;
    }
}
#endif

inline int mDiv(int x, int y){
    int returnVal = x/y;
    if (returnVal == 0){
        returnVal = 1;
    }
    return returnVal;
}

typedef unsigned char byte;

namespace beatOff{

    cTextBox::cTextBox() : cObject(){
        if(!TTF_WasInit() && TTF_Init() == -1){
            std::string mError = currentDateTime() + ": ";
            mError = "[textbox.cpp] - TTF Couldn't initialise (TTF Error: ";
            mError += TTF_GetError();
            mError += ")";
            setError(mError);
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
            compacted = 0;
        }
    }

    cTextBox::cTextBox(std::string inText, std::string fontLoc, int fontSize, int inX, int inY, int inW, int inH) : cObject(inX, inY, inW, inH){
        if(!TTF_WasInit() && TTF_Init() == -1){
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
            setText(inText);
            setFont(fontLoc);
            setTextSize(fontSize);
            compacted = 0;
        }
    }

    bool cTextBox::isCentered(){
        return centered;
    }

    bool cTextBox::isCompacted(){
        return compacted;
    }

    void cTextBox::centre(){
        centered = !(centered);
    }

    void cTextBox::compact(){
        compacted = !(compacted);
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
        if(compacted){
            return getFontHeight();
        }
        else{
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
                int tempWidth, numLines = 1, space = -1;
                std::string temp;
                TTF_Font* mFont = TTF_OpenFont(font.c_str(), textSize);

                for(int i = 0, j = text.size(); i < j; i++){
                    temp += text[i];
                    if(text[i] == ' '){
                        space = i;
                    }
                    TTF_SizeUTF8(mFont, temp.c_str(), &tempWidth, NULL);
                    if(tempWidth > w){
                        if(space < 0){
                            temp.pop_back();
                            i--;
                            numLines++;
                            temp.clear();
                        }
                        else{
                            for(int k = 0, l = i-space; k < l; k++){
                                temp.pop_back();
                                i--;
                            }
                            numLines++;
                            temp.clear();
                            space = -1;
                        }
                    }
                    else if(text[i] == '\n'){
                        temp.pop_back();
                        numLines++;
                        temp.clear();
                        space = -1;
                    }
                }
                if(temp.size() > 0 && temp[0] != '\n'){
                    numLines++;
                }
                renderedHeight = (TTF_FontHeight(mFont)*numLines) + (TTF_FontLineSkip(mFont)* (numLines-1));
                TTF_CloseFont(mFont);
            }
            return(renderedHeight);
        }
    }

    bool cTextBox::canFit(int incomingHeight){
        return(wrappedHeight() <= incomingHeight);
    }

    std::string cTextBox::getText(){
        return text;
    }

    int cTextBox::getFontHeight(){
        TTF_Font* mFont = TTF_OpenFont(font.c_str(), textSize);
        int rVal = TTF_FontHeight(mFont);
        TTF_CloseFont(mFont);
        return rVal;
    }

    void cTextBox::render(SDL_Renderer* mRenderer){
        if(!fileExists(font)){
            printf("No Font Declared: %s \n", font.c_str()); //Killing error
            exit(9001);
        }
        else{
            TTF_Font* mFont = TTF_OpenFont(font.c_str(), textSize);
            int tempY = -1, tempW, lineSkip = TTF_FontLineSkip(mFont);
            int space = -1;
            std::string temp;
            std::string text = this->text;
            std::vector<std::string> lines;
            SDL_Rect dRect;
            /* Set original dimensions of dRect */
            dRect.x = x;
            dRect.y = y;
            dRect.h = h;
            dRect.w = w;
            SDL_Texture* mTexture = NULL;
            SDL_Surface* mSurface = NULL;
            SDL_Colour mColour = {static_cast<Uint8>(textR), static_cast<Uint8>(textG), static_cast<Uint8>(textB), static_cast<Uint8>(textA)};

            if(this->h > 0){ //If we have a set height
                int expected = wrappedHeight(), tempW;
                if(this->h < expected){
                    if(compacted){
                        /* This is really bad because I'm changing the internal text then resetting
                         * it to it's original value -_-
                         */
                        if(this->compactedText.size()){
                            text = this->compactedText;
                        }
                        else{
                            temp = this->text;
                            this->text.erase(this->text.end()-3, this->text.end());
                            this->text += "...";
                            while(!this->canFit(this->h)){
                                this->text.erase(this->text.end()-4);
                            }
                            text = this->text;
                            this->compactedText = this->text;
                            this->text = temp;
                            temp.clear();
                        }
                    }
                    else{
                        std::string mWarning = currentDateTime() + ": ";
                        mWarning = "[textbox.cpp] Inputted height is too small by ";
                        mWarning += std::to_string(h - expected);
                        mWarning += " pixels!";
                        setWarning(mWarning);
                    }
                }
                else{
                    tempY = dRect.y + (dRect.h-expected)/2;
                    TTF_SizeUTF8(mFont, text.c_str(), &tempW, &expected);
                    if(dRect.w < tempW){
                        dRect.x += (tempW-w)/2;
                    }
                }
            }
            else{
                this->h = wrappedHeight();
                dRect.h = this->h;
            }

            for(int i = 0, j = text.size(); i < j; i++){
                temp += text[i];
                if(text[i] == ' '){
                    space = i;
                }
                TTF_SizeUTF8(mFont, temp.c_str(), &tempW, NULL);
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
                else if(text[i] == '\n'){
                    temp.pop_back();
                    lines.push_back(temp);
                    temp.clear();
                    space = -1;
                }
            }
            if(temp.size() > 0 && temp[0] != '\n'){
                lines.push_back(temp);
            }
            if(drawBox){
                SDL_SetRenderDrawColor(mRenderer, boxR, boxG, boxB, boxA);
                SDL_RenderFillRect(mRenderer, &dRect);
            }
            dRect.h = TTF_FontHeight(mFont);
            if(tempY > 0) dRect.y = tempY;
            for(int i = 0, j = lines.size(); i < j; i++){
                TTF_SizeUTF8(mFont, lines[i].c_str(), &dRect.w, &dRect.h);

                if(centered){
                    dRect.x = x + (w-dRect.w)/2; //It'll favour shifting towards the left by one pixel. w/e
                }
                else{
                    dRect.x = x;
                }
                dRect.y = y+(i*lineSkip);

                mSurface = TTF_RenderUTF8_Blended(mFont, lines[i].c_str(), mColour);
                mTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
                SDL_RenderCopy(mRenderer, mTexture, NULL, &dRect);
                SDL_FreeSurface(mSurface);
                SDL_DestroyTexture(mTexture);
            }
            TTF_CloseFont(mFont);
        }
    }

    void cWikiTextBox::render(SDL_Renderer* mRenderer){
        if(!fileExists(fontLoc.c_str())){
            printf("Font doesn't exist! \n");
            exit(500); //IDK
        }
        else{
            TTF_Font* mFont(fontLoc.c_str(), fontSize);
            int currX = 0, currY = 0;
            int currStyle = 0; 
            currStyle = TTF_STYLE_NORMAL; // Just keep or-ing the new thing.
            int lineskip =  TTF_FontLineSkip();
            int letterW = -1; //Will be updated on the fly anyway
            int letterH = -1; //Updated on the fly
            
            SDL_Colour mColour = {static_cast<Uint8>(textR), static_cast<Uint8>(textG), static_cast<Uint8>(textB), static_cast<Uint8>(textA)};

            currX = this->x;
            currY = this->y;
            /* Currently not doing nice text wrapping because reasons */

            for(unsigned int i = 0; i < this->text,size(); i++){
                switch(text[i]){
                    case '*':
                        {
                            if(text[i+1] == '*'){
                                i++;
                                currStyle ^= TTF_STYLE_BOLD;
                            }
                            else{
                                currStyle ^= TTF_STYLE_ITALIC;
                            }
                            break;
                        }

                    case '_':
                        {
                            if(text[i+1] == '_'){
                                i++;
                                currStyle ^= TTF_STYLE_UNDERLINE;
                                break;
                            }
                        }

                    default:
                        /* Render the glyph, move on */
                        TTF_SetFontStyle(mFont, currStyle);
                        TTF_SizeUTF8(mFont, text[i], &letterW, &letterH);
                        TTF_RenderUTF8_Blended(mFont, text[i], mCOlour);
                        currX += letterW;
                        if(currX > this->x + this->w){ //Too wide now.
                            currY += letterH; //Should be a constant thing anyway
                            currX = x;
                        }
                        break;
                }
            }
        }
    } 
}
