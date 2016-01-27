#include "contents.hpp"

//Lookup just for this file (I really wanna kill myself 'cus of this BADCODE
#define ND_FONT_LOOKUP(...) (*(config))["fontList"].find(__VA_ARGS__)->second["sauce"]

namespace beatOff{

    cNovelDetails::cNovelDetails(){
        this->loaded = 0;
        this->selection = -1;
        this->mTexture = NULL;
        this->mLog = new __logger::cLogger("logs/details.log");
        this->selection = -1;
        this->highlighted = 0;
        this->textureGen = 0;
    }

    cNovelDetails::cNovelDetails(__logger::cLogger* mLog){
        this->loaded = 0;
        this->selection = 0;
        this->mTexture = NULL;
        this->mLog = mLog;
        this->selection = -1;
        this->textureGen = 0;
    }

    void cNovelDetails::setRect(SDL_Rect inRect){
        sRect = inRect; //The "viewport" of the texture
        sRect.x = 0;
        sRect.y = 0;
        setPos(inRect.x, inRect.y); //The position to render to on screen
        /* Like all things, height is variable, its just the width that stays the same */
        setSize(inRect.h, inRect.w); 
    }

    std::string cNovelDetails::getSelected(){ //Returns the chapter location
        if(this->selection > 0){
            return this->contents[selection].second.first;
        }
        else{ //Sanity check anyway just to be sure
            this->state = broken;
            this->err = "[novelDetails.cpp] Error: getSelected was called even though there isn't a valid novel (maybe I sent the wrong signal?)";
            return "";
        }
    }

    std::string cNovelDetails::getChapName(){
        return ((cButton*)this->contents[selection].first)->getText();
    }

    std::string cNovelDetails::getChapID(){
        return this->novelID[((cButton*)this->contents[selection].first)->getText()];
    }

    std::string cNovelDetails::getRevID(){
        return this->chapRevID[((cButton*)this->contents[selection].first)->getText()];
    }

    void cNovelDetails::openNovel(
            std::string sauce, SDL_Renderer* mRenderer, 
            std::map<std::string, std::multimap<std::string, std::map<std::string, std::string> > >* config //More profitable to just pass the whole config in instead of doing stupid shit
            ){ 
        try{
            /* Clearing out old textures */
            if(mTexture){
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
            }

            /* Grabbing Volume + Chapter list */
            std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string> > > > volumes;

            pugi::xml_document doc;
            bool hack = 0;
            doc.load_file(sauce.c_str()); //Ignoring return value (I'm a NAUGHTY BOY elegiggle)
            pugi::xml_node mainNode = doc.child("novel");
            std::string title = mainNode.child("info").attribute("title").value();
            std::string author = mainNode.child("info").attribute("author").value();
            std::string synopsis = mainNode.child("synopsis").text().get(); //Can do this, because only one text field in synopsis node

            /* Grab the image location to render 
             * This only grabs the cover image from the first volume. I could
             * edit it so that it would include the first cover image that
             * exists, but that is for later ~~~~~~~~~~~~~
             * ***************************************************************/
            std::string frontLoc;

            for(auto currNode: mainNode.children("volume")){
                std::vector<std::pair<std::string, std::string> > chapterDetails;
                if(!hack) frontLoc = currNode.attribute("image").value();
                for(auto chapterNode: currNode.children("chapter")){
                    chapterDetails.push_back(std::make_pair(chapterNode.attribute("title").value(), chapterNode.attribute("location").value())); //Wew!
                    this->novelID[chapterNode.attribute("title").value()] = chapterNode.attribute("id").value();
                    this->chapRevID[chapterNode.attribute("title").value()] = chapterNode.attribute("revid").value();
                }
                volumes.push_back(std::make_pair(currNode.attribute("title").value(), chapterDetails));
            }

            /* TIME TO RENDER TO A TEXTURE !!! */

            /* Calculate the height and create objects beforehand, because texture height must be
             * declared beforehand -_- */

            cObject* newObject; //A temporary textbox that will only be used for the "wrapped height" method
            unsigned int mHeight = 0;

            newObject = new cTextBox(
                    title, 
                    ND_FONT_LOOKUP((*(config))["novelDetails"].find("title")->second["font"]),
                    std::stoi((*(config))["novelDetails"].find("title")->second["size"]),
                    0, mHeight,this->sRect.w
                    );
            /* Title */
            if(!((cTextBox*)newObject)->isCentered()) ((cTextBox*)newObject)->centre();
            mHeight += ((cTextBox*)newObject)->wrappedHeight();
            this->contents.push_back(std::make_pair(newObject, std::make_pair(std::string("__NONE__"), mHeight))); //__NONE__ to stand for unclickable

            /* Image */
            if(!(*(config))["novelDetails"].count("image"))
                newObject = new cImage(
                        frontLoc, 
                        0, 
                        mHeight, 
                        -1, 
                        this->sRect.w
                        ); //Don't want to exceed the width of the screen
            else
                newObject = new cImage(
                        frontLoc, 
                        0, 
                        mHeight, 
                        std::stoi((*(config))["novelDetails"].find("image")->second["h"]), 
                        std::stoi((*(config))["novelDetails"].find("image")->second["w"])
                        ); //Trusting the config man to not be stupid and allocate W to be greater than sRect.w (Possibly want to do a min of this)
            mHeight += ((cImage*)newObject)->getSize().first;
            this->contents.push_back(std::make_pair(newObject, std::make_pair(std::string("__NONE__"), mHeight)));

            /* Synopsis */
            newObject = new cTextBox(
                    synopsis,
                    ND_FONT_LOOKUP((*(config))["novelDetails"].find("synopsis")->second["font"]),
                    std::stoi((*(config))["novelDetails"].find("synopsis")->second["size"]),
                    0, mHeight, this->sRect.w
                    );
            mHeight += ((cTextBox*)newObject)->wrappedHeight();
            this->contents.push_back(std::make_pair(newObject, std::make_pair(std::string("__NONE__"), mHeight)));

            /* Volume/chapter details */
            for(auto volume : volumes){
                newObject = new cTextBox(
                        volume.first,
                        ND_FONT_LOOKUP((*(config))["novelDetails"].find("volume")->second["font"]),
                        std::stoi((*(config))["novelDetails"].find("volume")->second["size"]),
                        0, mHeight, this->sRect.w
                        );
                mHeight += ((cTextBox*)newObject)->wrappedHeight();
                this->contents.push_back(std::make_pair(newObject, std::make_pair(std::string("__NONE__"), mHeight)));
                for(auto chapter: volume.second){
                    newObject = new cButton(
                            chapter.first,
                            ND_FONT_LOOKUP((*(config))["novelDetails"].find("chapter")->second["font"]),
                            std::stoi((*(config))["novelDetails"].find("volume")->second["size"]),
                            0, mHeight, this->sRect.w
                            );
                    if(!((cButton*)newObject)->isCompacted()) ((cButton*)newObject)->compact();
                    mHeight += ((cButton*)newObject)->wrappedHeight();
                    this->contents.push_back(std::make_pair(newObject, std::make_pair(chapter.second, mHeight)));
                }
            }

            /* Create the texture first */
            mTexture = SDL_CreateTexture(
                    mRenderer,
                    SDL_PIXELFORMAT_UNKNOWN,
                    SDL_TEXTUREACCESS_TARGET,
                    w,
                    mHeight //Height is variable though so I had to pre-calc it =.=
                    );
            SDL_SetRenderTarget(mRenderer, mTexture);
            SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
            SDL_RenderClear(mRenderer);
            for(int i = 0; i < this->contents.size(); i++){
                this->contents[i].first->render(mRenderer);
            }

            /* Return the renderer to render to the window again */
            SDL_SetRenderTarget(mRenderer, NULL);

            /* Set default starting pos to 0, 0. Set here, because things will always be re-loaded */
            this->sRect.x = 0;
            this->sRect.y = 0;
            this->loaded = 1;
            this->textureGen = 1;
        }
        catch(mException& e){
            setError(e.what()); 
            printf("%s \n", e.what());
        }
    }

    void cNovelDetails::move(int dx, int dy){ //DOES NOT SANITY CHECK!!! (Should be done beforehand anyway)
        /*Ignoring dx at the momemnt, until I can get zoom working. */
        sRect.y += dy;
        //sRect.x += dx;
    }

    void cNovelDetails::moveSelection(int ds){
        if(ds){
            int diff = (ds > 0 ? -1 : 1);
            ds = ds > 0 ? ds : ds*-1;
            unsigned int last = this->selection; //In case there isn't anything to go to
            while(ds){ //Can afford shitty algo since tiny n
                this->selection += diff;
                if(this->selection < 0 or this->selection >= this->contents.size()){ //No other valid "selecatble" option
                    this->selection = last;
                    break;
                }
                else if(this->contents[this->selection].second.first != "__NONE__"){ //Check selectable
                    ds--;
                }
                //else do nothing
            }
            if(last != this->selection and this->selection > 0){
                ((cButton*)this->contents[this->selection].first)->select();
                ((cButton*)this->contents[last].first)->deselect();
            }
            this->textureGen = 0;
        }
    }

    void cNovelDetails::genTexture(SDL_Renderer* mRenderer){
        SDL_SetRenderTarget(mRenderer, mTexture);
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
        SDL_RenderClear(mRenderer);
        for(auto object : this->contents){
            object.first->render(mRenderer);
        }
        SDL_SetRenderTarget(mRenderer, NULL);
        this->textureGen = 1;
    }

    void cNovelDetails::render(SDL_Renderer* mRenderer){ //TODO: Verify that this still works
        if(!this->loaded){
            std::string mError = currentDateTime() = ": ";
            mError += "[novelDetails.cpp] File wasn't loaded before calling render \n";
            printf("%s \n", mError.c_str());
            throw(mException(mError));
        }
        else{
            if(!this->textureGen) genTexture(mRenderer);
            SDL_Rect dRect;
            dRect.x = x;
            dRect.y = y;
            /* sRect(h/w) == dRect(h/w) */
            dRect.h = h;
            dRect.w = w;
            SDL_RenderCopy(mRenderer, mTexture, &sRect, &dRect);
        }
    }

    void cNovelDetails::handleUserMouse(int x, int y, int button, bool isDown){
        int mx = x - this->x, my = y - this->y;
        if(!isDown){ //Trigger event on rising edge
            switch(button){
                case SDL_BUTTON_LEFT:
                    /* Work out what I'm hovering over, then check if it's a selectable, if it is,
                     * highlight it and trigger the flag, otherwise don't do anything */
                    for(int i = 0; i < this->contents.size(); i++){
                        if(this->contents[i].second.second > y){
                            if(this->contents[i].second.first == "__NONE__"){
                                this->highlighted = 0;
                            }
                            else{
                                this->selection = i;
                                this->state = go;
                            }
                            break;
                        }
                    }
                    break;

                case SDL_BUTTON_RIGHT:
                    /* Don't handle ATM */
                    break;

                case SDL_BUTTON_MIDDLE:
                    this->freeScroll = !this->freeScroll; //Flip the bit
                    if(this->freeScroll){ //Only set these variables if freeScroll is active
                        this->fsX = mx;
                        this->fsY = my;
                    }
                    break;

                case -1:
                    if(this->freeScroll){
                        /* Since we dont' move X around */
                        move(0, my-fsY);
                    }
                    else{
                        /* Check if I'm hovering over a selectable. If I am, then highlight it,
                         * otherwise, don't
                         */
                        /* Find what object I'm hovering over right now */
                        for(int loc = 0; loc < this->contents.size(); loc++){
                            if(this->contents[loc].second.second > y){
                                if(this->contents[loc].second.first != "__NONE__"){
                                    /* Over hoverable */
                                    ((cButton*)this->contents[this->selection].first)->deselect();
                                    this->selection = loc;
                                    ((cButton*)this->contents[this->selection].first)->select();
                                    this->textureGen = 0;
                                }
                                break;
                            }
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }

    void cNovelDetails::handleUserKeyboard(std::string id, bool isPressed, unsigned int bitmask){
        /* Add in ID cases as we go along. On second thoughts, this wasn't such
         * a good idea.......
         */
        if(isPressed){//Not handling hold downs -> That's for mouse anyway
            if(this->highlighted){
                if(id == "up"){
                    moveSelection(-1); //Sanity checks should be done in function
                    this->textureGen = 0;
                }
                else if(id == "down"){
                    moveSelection(1);
                    this->textureGen = 0;
                }
                else if(id == "go"){
                    /* Selected that object */
                    this->state = go;
                }
            }
        }
        else{
            /* Scroll up/down */
            if(id == "up"){
                move(0, std::max(-20, 0-this->sRect.y)); //Inherently does a sanity check
            }
            else if(id == "down"){
                move(0, std::min(20, this->sRect.h));
            }
            else if(id == "go"){
                /* Check if something has been selected, then DROP THE BASS */
                if(this->selection > 0){
                    this->state = go;
                }
            }
        }
    }

    void cNovelDetails::handleUserScroll(int dx, int dy){
        move(0, std::max(dy*25*-1, -1*sRect.y));
    }

}
