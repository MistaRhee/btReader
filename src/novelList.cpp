#include "contents.hpp"

namespace beatOff{

    cNovelList::cNovelList(){
        this->selected = 0;
        /* Setting "defaults" just in case I forget to set them beforehand */
        this->fontSize = 25;
        this->textColour.r = 0;
        this->textColour.g = 0;
        this->textColour.b = 0;
        this->textColour.a = 0;
        this->backColour.r = 255;
        this->backColour.g = 255;
        this->backColour.b = 255;
        this->backColour.a = 255;
        setFont("system/fonts/droidsansfallback.ttf"); //Should be default
        setFontSize(20);

        /* Handles the texturing of the novel list */
        this->mTexture = NULL;
        this->textureGen = 0;
        this->mouseMoved = 0;
        this->novelHeight = 20; //Defaulted to this
    }
    
    void cNovelList::setRect(SDL_Rect inRect){
        this->sauceRect = inRect;
        setPos(inRect.x, inRect.y);
        setSize(inRect.h, inRect.w);
    }

    void cNovelList::addNovel(std::string in, int novelHeight){
        /* Variable height makes my OCD go insane */
        cTextBox newText;
        this->novelHeight = novelHeight;
        newText.setText(in);
        newText.setTextSize(fontSize);
        newText.setPos(x, h);
        if(newText.canFit(novelHeight)){
            newText.setSize(w, novelHeight);
        }
        else{
            /* Scale font down so it can actually fit inside the requested
             * height */
            bool canSqueeze = 0;
            for(int i = 1; i < (fontSize*3)/4; i++){ //Only will scale down to 75%
                newText.setTextSize(fontSize-i);
                if(newText.canFit(novelHeight)){
                    newText.setSize(w, novelHeight);
                    canSqueeze = 1;
                    break;
                }
            }
            if(!canSqueeze){
                /* If I can't squeeze it in by making the font smaller, cut the
                 * title off to make it fit */
                newText.setTextSize(fontSize);
                in.erase(in.end()-3, in.end());
                in += "...";
                while(true){
                    in.erase(in.end()-4); //Erase the back character (that isn't elipsis
                    if(newText.canFit(novelHeight)) break;
                }
                newText.setText(in);
            }
        }
        newText.showBox();
        newText.setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
        newText.setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);
        newText.setFont(fontLoc);
        fprintf(stderr, "%s \n", fontLoc.c_str());
        newText.centre(); //Because text centering is nice
        this->mNovels.push_back(newText);
        h += novelHeight;
    }

    void cNovelList::moveSelection(int ds){
        /* Check if the mouse has moved -> if it has, change selection based
         * off absolute mouse position, rather than old selection position
         */
        this->textureGen = 0;
        this->mNovels[this->selected].setTextCol(this->textColour.r, this->textColour.g, this->textColour.b, this->textColour.a);
        this->mNovels[this->selected].setBoxCol(this->backColour.r, this->backColour.g, this->backColour.b, this->backColour.a);
        this->selected += ds;
        this->mNovels[this->selected].setTextCol(255 - this->textColour.r, 255 - this->textColour.g, 255 - this->textColour.b, 255 - this->textColour.a);
        this->mNovels[this->selected].setBoxCol(255 - this->backColour.r, 255 - this->backColour.g, 255 - this->backColour.b, 255 - this->backColour.a);

        /* Check to see if selection is on screen, if it isn't, move screen
         * such that it is */
        int reqY = 0; //Required value of Y to make the novel be see-able (is this a word?)
        reqY += this->novelHeight*(this->selected+1);
        reqY -= this->h-this->novelHeight; //I'm displaying a bunch of other stuff anyway
        this->sauceRect.y = reqY;
    }

    void cNovelList::genTexture(SDL_Renderer* mRenderer){
        SDL_DestroyTexture(mTexture);
        /* Calculate the required height of the texture */
        int mHeight = this->novelHeight*this->mNovels.size();

        /* Render shit to a texture */
        SDL_SetRenderTarget(mRenderer, this->mTexture);
        this->mTexture = SDL_CreateTexture(
                mRenderer,
                SDL_PIXELFORMAT_UNKNOWN,
                SDL_TEXTUREACCESS_TARGET,
                w,
                mHeight
                );
        for(auto it = this->mNovels.begin(); it != this->mNovels.end(); ++it){
            /* This is so bad/lazy */
            it->render(mRenderer);
        }

        /* Return renderer to render to the window again */
        SDL_SetRenderTarget(mRenderer, NULL);
        this->textureGen = 1;
    }

    std::string cNovelList::getSelected(){
        return this->mNovels[this->selected].getText();
    }

    void cNovelList::render(SDL_Renderer* mRenderer){
        if(!textureGen) genTexture(mRenderer);
        SDL_Rect dRect;
        dRect.x = x;
        dRect.y = y;
        dRect.h = h;
        dRect.w = w;
        /* Sauce rect h and w should be equal to dRect */
        SDL_RenderCopy(mRenderer, mTexture, &sauceRect, &dRect);
    }
    
    void cNovelList::handleUserKeyboard(std::string id, bool isPressed, unsigned int bitmask){
        /* Add in ID cases as we go along. On second thoughts, this wasn't such
         * a good idea.......
         */
        if(id == "up"){
            /* Move selection up */
            this->selected ++;
            if(this->selected >= this->mNovels.size()) this->selected = this->mNovels.size();
        }
        else if (id == "down"){
            /* Move selection down */
            this->selected --;
            if(this->selected < 0) this->selected = 0;
        }
        else if (id == "go"){
            /* Selected that object */
            this->mState = go;
        }
    }

    void cNovelList::handleUserScroll(int dx, int dy){
        move(0, dy); //No horizontal scrolling
    }

    void cNovelList::handleUserMouse(int x, int y, int mouseType, bool isPressed){
        /* Time to do funky math to work out what they clicked/hovering over
         * right now */
        /* Note to self: if mouseType = -1, then this is a mouse MOTION event
         * */
        switch(mouseType){
            case SDL_BUTTON_LEFT:
                /* Left click */
                break;

            case SDL_BUTTON_RIGHT:
                /* Right click */
                break;

            case SDL_BUTTON_MIDDLE:
                /* Middle mouse */
                break;

            case -1: //My non-button case i.e. Mouse Motion
                this->mouseMoved = 1; //Now highlighting what the mouse is hovering over, not what they keyboard was on
                break;

            default:
                /* Should only reach here on non-conventional mouse keys which I don't handle ROFL */
                break;
        }

    }

}
