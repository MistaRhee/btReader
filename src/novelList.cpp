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
        setFont(std::string("system/fonts/default.ttf")); //Should be defaut
        setFontSize(20);

        /* Handles the texturing of the novel list */
        this->mTexture = NULL;
        this->textureGen = 0;
        this->novelHeight = 20; //Defaulted to this
        this->freeScroll = 0;
        this->fsX = -1;
        this->fsY = -1;
    }

    cNovelList::cNovelList(__logger::cLogger* mLog){
        this->mLog = mLog;
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
        setFont(std::string("system/fonts/default.ttf")); //Should be defaut
        setFontSize(20);

        /* Handles the texturing of the novel list */
        this->mTexture = NULL;
        this->textureGen = 0;
        this->novelHeight = 20; //Defaulted to this
        this->freeScroll = 0;
        this->fsX = -1;
        this->fsY = -1;
    }

    cNovelList::~cNovelList(){}

    void cNovelList::setRect(SDL_Rect inRect){
        this->sRect = inRect; //To set appropriate height and width.
        this->sRect.x = 0;
        this->sRect.y = 0;
        setPos(inRect.x, inRect.y);
        setSize(inRect.h, inRect.w);
    }

    void cNovelList::addNovel(std::string in, int novelHeight, std::string fontLoc){
        this->mLog->log(std::string("[novelList.cpp] Info: Adding novel ") + in);

        /* Flag that the novelList texture must be regenerated */
        this->textureGen = 0;

        /* Variable height makes my OCD go insane */
        if(!this->mNovels.count(in)){
            cTextBox newText;
            this->novelHeight = novelHeight;
            newText.setText(in);
            newText.setFont(fontLoc);
            newText.setTextSize(fontSize);
            newText.setPos(x, h);
            if(newText.canFit(novelHeight)){
                newText.setSize(w, novelHeight);
            }
            else{
                /* If I can't squeeze it in by making the font smaller, cut the
                 * title off to make it fit */
                in.erase(in.end()-3, in.end());
                in += "...";
                while(true){
                    in.erase(in.end()-4); //Erase the back character (that isn't elipsis
                    if(newText.canFit(novelHeight)) break;
                }
                newText.setText(in);
            }
            newText.showBox();
            newText.setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
            newText.setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);
            newText.setFont(fontLoc);
            newText.centre(); //Because text centering is nice
            this->mNovels[in] = newText;
            h += novelHeight;

            /* Insert the novel name in the appropriate location */
            this->novelNames.insert(this->novelNames.begin()+std::distance(this->mNovels.begin(), this->mNovels.find(in)), in);
        }
        else this->mLog->log(std::string("[novelList.cpp] Info: Novel already exists! Ignoring \n"));
    }

    void cNovelList::move(int dx, int dy){ //Overwriting move function to move the source rect anyway
        /* Ignoring dx simply because it shouldn't exist anyway */
        this->sRect.y += dy;
    }

    void cNovelList::moveSelection(int ds){
        /* Move it according to ds -> DOES NOT DO SANITY CHECK */
        this->textureGen = 0;
        this->mNovels[this->novelNames[this->selected]].invert();
        this->selected += ds;
        this->mNovels[this->novelNames[this->selected]].invert();

        /* Check to see if selection is on screen, if it isn't, move screen
         * such that it is */
        int reqY = 0; //Required value of Y to make the novel be see-able (is this a word?)
        reqY += this->novelHeight*(this->selected+1);
        reqY -= this->h-this->novelHeight; //I'm displaying a bunch of other stuff anyway
        this->sRect.y = reqY;
    }

    void cNovelList::genTexture(SDL_Renderer* mRenderer){
        printf("Generating Texture\n");
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
            it->second.render(mRenderer);
        }

        /* Return renderer to render to the window again */
        SDL_SetRenderTarget(mRenderer, NULL);
        this->textureGen = 1;
    }

    std::string cNovelList::getSelected(){
        return this->mNovels[this->novelNames[this->selected]].getText();
    }

    void cNovelList::render(SDL_Renderer* mRenderer){
        if(!textureGen) genTexture(mRenderer);
        SDL_Rect dRect;
        dRect.x = x;
        dRect.y = y;
        dRect.h = h;
        dRect.w = w;
        /* Sauce rect h and w should be equal to dRect */
        SDL_RenderCopy(mRenderer, mTexture, &sRect, &dRect);
    }

    void cNovelList::handleUserKeyboard(std::string id, bool isPressed, unsigned int bitmask){
        /* Add in ID cases as we go along. On second thoughts, this wasn't such
         * a good idea.......
         */
        if(id == "up"){
            /* Move selection up */
            if(this->selected < this->mNovels.size()-1){ //I can still scroll down (increase the selection number)
                moveSelection(1);
            }
            /* Otherwise ignore it since I'm at the bottom anyway -> NO WRAPPING AROUND THE LIST */
        }
        else if (id == "down"){
            /* Move selection down */
            if(this->selected > 0){ //I can still move up (decrase the selection number)
                moveSelection(-1);
            }
        }
        else if (id == "go"){
            /* Selected that object */
            this->state = go;
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
                {
                    /* Left click */
                    /* Work out what I'm hovering over */
                    int currHeight = this->sRect.y + y;
                    int novelsDown = currHeight/this->novelHeight;
                    /* Check if the top is cutting off a novel block */
                    if(!currHeight%this->novelHeight){
                        novelsDown++;
                    }
                    this->mNovels[this->novelNames[this->selected]].invert();
                    this->selected = novelsDown;
                    this->mNovels[this->novelNames[this->selected]].invert();

                    /* Flag that it has done something */
                    this->state = go;
                    break;
                }

            case SDL_BUTTON_RIGHT:
                /* Right click */
                //Doing nothing with right click ATM (maybe later)
                break;

            case SDL_BUTTON_MIDDLE:
                /* Middle mouse */
                //Start free scrolling
                this->freeScroll = !this->freeScroll; //Flip the bit
                if(this->freeScroll){ //Only set these variables if freeScroll is active
                    this->fsX = x;
                    this->fsY = y;
                }
                break;

            case -1: //My non-button case i.e. Mouse Motion
                {
                    if(this->freeScroll){
                        /* Since we dont' move X around */
                        move(0, y-fsY);
                    }
                    else{
                        /* Work out what I'm hovering over */
                        int currHeight = this->sRect.y + y;
                        int novelsDown = currHeight/this->novelHeight;
                        /* Check if the top is cutting off a novel block */
                        if(!currHeight%this->novelHeight){
                            novelsDown++;
                        }
                        this->mNovels[this->novelNames[this->selected]].invert();
                        this->selected = novelsDown;
                        this->mNovels[this->novelNames[this->selected]].invert();
                    }
                    break;
                }

            default:
                /* Should only reach here on non-conventional mouse keys which I don't handle ROFL */
                break;
        }

    }

}
