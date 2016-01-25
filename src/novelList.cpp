#include "contents.hpp"

inline int fastMax(int x, int y) { return (((y-x)>>(31))&(x^y))^y; }

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
        this->mLog = new __logger::cLogger("logs/list.log");
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

    void cNovelList::clear(){
        this->mNovels.clear();
        this->novelNames.clear();
    }

    void cNovelList::addNovel(std::string in, int fontSize, std::string fontLoc){
        this->mLog->log(std::string("[novelList.cpp] Info: Adding novel ") + in);


        /* Variable height makes my OCD go insane */
        if(!this->mNovels.count(in)){
            /* Flag that the novelList texture must be regenerated */
            this->textureGen = 0;
            cButton newText(in, fontLoc, fontSize, 0, this->mNovels.size()*this->novelHeight, w); //Should be generated from corner to corner of the texture

            this->novelHeight = newText.getFontHeight(); //Should never change, but just in case I guess..?
            newText.setSize(this->novelHeight, w);

            newText.setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
            newText.setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);

            newText.compact();

            /* Update local variables */
            this->mNovels[in] = newText;

            /* Insert the novel name in the appropriate location */
            this->novelNames.insert(this->novelNames.begin()+std::distance(this->mNovels.begin(), this->mNovels.find(in)), in);
        }
        else this->mLog->log(std::string("[novelList.cpp] Info: Novel already exists! Ignoring \n"));
    }

    void cNovelList::move(int dx, int dy){ //Overwriting move function to move the source rect anyway
        /* Ignoring dx simply because it shouldn't exist anyway */
        this->sRect.y += dy;
        int maxHeight = this->mNovels.size()*this->novelHeight;
        if(this->sRect.y > maxHeight-this->sRect.h) this->sRect.y = maxHeight-this->sRect.h;
        else if(this->sRect.y < 0) this->sRect.y = 0;
    }

    void cNovelList::moveSelection(int ds){
        /* Move it according to ds -> DOES NOT DO SANITY CHECK */
        this->textureGen = 0;
        this->mNovels[this->novelNames[this->selected]].deselect();
        this->selected += ds;

        this->mNovels[this->novelNames[this->selected]].select();

        /* Check to see if selection is on screen, if it isn't, move screen
         * such that it is */
        int reqY = 0; //Required value of Y to make the novel be see-able (is this a word?)
        reqY += this->novelHeight*(this->selected+1);
        reqY -= this->h-this->novelHeight; //I'm displaying a bunch of other stuff anyway
        this->sRect.y = fastMax(0, reqY);
    }

    void cNovelList::genTexture(SDL_Renderer* mRenderer){
        SDL_DestroyTexture(mTexture);
        if(this->mNovels.size()){
            /* Calculate the required height of the texture */
            int mHeight = this->novelHeight*this->mNovels.size();

            /* Render shit to a texture */
            this->mTexture = SDL_CreateTexture(
                    mRenderer,
                    SDL_PIXELFORMAT_UNKNOWN,
                    SDL_TEXTUREACCESS_TARGET,
                    w,
                    mHeight
                    );
            SDL_SetRenderTarget(mRenderer, this->mTexture);
            SDL_SetRenderDrawColor(mRenderer, this->backColour.r, this->backColour.g, this->backColour.b, this->backColour.a);
            SDL_RenderClear(mRenderer);
            for(auto it = this->mNovels.begin(); it != this->mNovels.end(); ++it){
                /* This is so bad/lazy */
                it->second.render(mRenderer);
            }

            /* Return renderer to render to the window again */
            SDL_SetRenderTarget(mRenderer, NULL);
            this->textureGen = 1;
        }
        else{
            this->mTexture = SDL_CreateTexture(
                    mRenderer, 
                    SDL_PIXELFORMAT_UNKNOWN,
                    SDL_TEXTUREACCESS_TARGET,
                    w,
                    this->h
                    );
            SDL_SetRenderTarget(mRenderer, this->mTexture);
            SDL_SetRenderDrawColor(mRenderer, this->backColour.r, this->backColour.g, this->backColour.b, this->backColour.a);
            SDL_RenderClear(mRenderer);
            beatOff::cTextBox updatingBox("btReader is currently updating (For more details, check the updates.log)", "system/fonts/default.ttf", 25, 0, 0, this->w);
            updatingBox.render(mRenderer);
            SDL_SetRenderTarget(mRenderer, NULL);
            this->textureGen = 1;
        }
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
        if(dRect.h > this->mNovels.size()*this->novelHeight) dRect.h = this->mNovels.size()*this->novelHeight; //To stop stretching when there isn't enough data
        /* Sauce rect h and w should be equal to dRect */
        SDL_RenderCopy(mRenderer, mTexture, &sRect, &dRect);
    }

    void cNovelList::handleUserKeyboard(std::string id, bool isPressed, unsigned int bitmask){
        /* Add in ID cases as we go along. On second thoughts, this wasn't such
         * a good idea.......
         */
        if(isPressed){//Not handling hold downs -> That's for mouse anyway
            if(id == "up"){
                /* Move selection up */
                if(this->selected > 0){ //I can still move up (decrase the selection number)
                    moveSelection(-1);
                }
                /* Otherwise ignore it since I'm at the bottom anyway -> NO WRAPPING AROUND THE LIST */
            }
            else if (id == "down"){
                /* Move selection down */
                if(this->selected < this->mNovels.size()-1){ //I can still scroll down (increase the selection number)
                    moveSelection(1);
                }
            }
            else if (id == "go"){
                /* Selected that object */
                this->state = go;
            }
        }
    }

    void cNovelList::handleUserScroll(int dx, int dy){
        move(0, dy*this->novelHeight*-1); //No horizontal scrolling
        /* Recalculate what the mouse is hovering over */
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        mx -= this->x;
        my -= this->y;
        this->textureGen = 0;
        int currHeight = this->sRect.y + my;
        int novelsDown = currHeight/this->novelHeight;
        /* Check if the top is cutting off a novel block */
        if(!currHeight%this->novelHeight){
            novelsDown++;
        }
        this->mNovels[this->novelNames[this->selected]].deselect();
        this->selected = novelsDown;
        this->mNovels[this->novelNames[this->selected]].select();
    }

    void cNovelList::handleUserMouse(int mx, int my, int mouseType, bool isPressed){
        /* Time to do funky math to work out what they clicked/hovering over
         * right now */
        /* Note to self: if mouseType = -1, then this is a mouse MOTION event
         * */
        /* Adjust mx and my due to the actual rect being offset */
        my -= this->y;
        mx -= this->x;
        if(!isPressed){ //Does something on mouse release
            switch(mouseType){
                case SDL_BUTTON_LEFT: 
                    {
                        /* Left click */
                        /* Flag that I need to regenerate the texture */
                        this->textureGen = 0;

                        /* Work out what I'm hovering over */
                        int currHeight = this->sRect.y + my;
                        int novelsDown = currHeight/this->novelHeight;
                        /* Check if the top is cutting off a novel block */
                        if(!currHeight%this->novelHeight){
                            novelsDown++;
                        }
                        if(novelsDown < this->mNovels.size()){
                            this->mNovels[this->novelNames[this->selected]].deselect();
                            this->selected = novelsDown;
                            this->mNovels[this->novelNames[this->selected]].select();

                            /* Flag that it has done something */
                            this->state = go;
                        }
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
                        this->fsX = mx;
                        this->fsY = my;
                    }
                    break;

                case -1: //My non-button case i.e. Mouse Motion
                    {
                        if(this->freeScroll){
                            /* Since we dont' move X around */
                            move(0, my-fsY);
                        }
                        else{
                            /* Work out what I'm hovering over */
                            this->textureGen = 0;
                            int currHeight = this->sRect.y + my;
                            int novelsDown = currHeight/this->novelHeight;
                            /* Check if the top is cutting off a novel block */
                            if(!currHeight%this->novelHeight){
                                novelsDown++;
                            }
                            /* Do sanity check to see if we have novels or not */
                            if(novelsDown < this->mNovels.size()){
                                this->mNovels[this->novelNames[this->selected]].deselect();
                                this->selected = novelsDown;
                                this->mNovels[this->novelNames[this->selected]].select();
                            }
                        }
                        break;
                    }

                default:
                    /* Should only reach here on non-conventional mouse keys which I don't handle ROFL */
                    break;
            }

        }
    }

}
