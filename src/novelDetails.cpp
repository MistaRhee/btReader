#include "contents.hpp"

//Lookup just for this file (I really wanna kill myself 'cus of this BADCODE
#define ND_FONT_LOOKUP(...) config["fontList"].find(__VA_ARGS__)->second["sauce"]

namespace beatOff{

    cNovelDetails::cNovelDetails(){
        loaded = 0;
        selection = 0;
        mTexture = NULL;
        this->mLog = new __logger::cLogger("logs/details.log");
        this->selected = -1;
    }

    cNovelDetails::cNovelDetails(__logger::cLogger* mLog){
        loaded = 0;
        selection = 0;
        mTexture = NULL;
        this->mLog = mLog;
        this->selected = -1;
    }

    void cNovelDetails::setRect(SDL_Rect inRect){
        sauceRect = inRect; //The "viewport" of the texture
        setPos(inRect.x, inRect.y); //The position to render to on screen
        /* Like all things, height is variable, its just the width that stays the same */
        setSize(inRect.h, inRect.w); 
    }

    std::string cNovelDetails::getSelected(){ //Returns the chapter location
        if(this->selected > 0){
            return this->objects[selected].second.first;
        }
        else{ //Sanity check anyway just to be sure
            this->state = broken;
            this->err = "[novelDetails.cpp] Error: getSelected was called even though there isn't a valid novel (maybe I sent the wrong signal?)";
            return "";
        }
    }

    void cNovelDetails::openNovel(
            std::string sauce, SDL_Renderer* mRenderer, 
            std::map<std::string, std::multimap<std::string, std::map<std::string, std::string> > >& config //More profitable to just pass the whole config in instead of doing stupid shit
            ){ //TODO: WIP. Just need to pre-calculate the height then render it to a texture
        try{
            /* Clearing out old textures */
            if(mTexture){
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
            }

            /* Grabbing Volume + Chapter list */
            std::vector<std::vector<std::pair<std::string, std::string> > > volumes;

            pugi::xml_document doc;
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
            std::string frontLoc = mainNode.child("volume").attribute("image").value(); //TODO: VERIFY THIS

            for(auto currNode: mainNode.children("volume")){
                std::vector<std::pair<std::string, std::string> > chapterDetails;
                for(auto chapterNode: currNode.children("chapter")){
                    chapterDetails.push_back(std::make_pair(chapterNode.attribute("title").value(), chapterNode.attribute("location").value())); //Wew!
                }
                volumes.push_back(chapterDetails);
            }
            loaded = 1;
            
            /* TIME TO RENDER TO A TEXTURE !!! */

            /* Calculate the height and create objects beforehand, because texture height must be
             * declared beforehand -_- */

            cObject* newObject; //A temporary textbox that will only be used for the "wrapped height" method
            newObject = new cTextBox(
                    title, 
                    ND_FONT_LOOKUP(config["novelDetails"].find("title")->second["font"]),
                    std::stoi(config["novelDetails"].find("title")->second["size"]),
                    );
            cButton newText(in, fontLoc, fontSize, 0, this->mNovels.size()*this->novelHeight, w); //Should be generated from corner to corner of the texture
            unsigned int mHeight = 0;
            /* Title */
            cTextBox* t = (cTextBox*) newObject;
            t->setText(title);
            t->setFont(ND_FONT_LOOKUP(config["novelDetails"].find("title")->second["font"]));
            t->setSize(std::stoi(config["novelDetails"].find("title")->second["size"]));
            t->setPos(0, mHeight);
            t->setSize(-1, this->sRect.w);
            t = NULL;
            mHeight += t->wrappedHeight();
            this->contents.push_back(std::make_pair(newObject, std::make_pair(std::string("__NONE__"), mHeight))); //__NONE__ to stand for unclickable
            t = NULL; // Just to be safe

            /* Image */
            if(!config["novelDetails"].count("image"))
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
                        std::stoi(config["novelDetails"].find("image")->second["h"]), 
                        std::stoi(config["novelDetails"].find("image")->second["w"])
                        ); //Trusting the config man to not be stupid and allocate W to be greater than sRect.w (Possibly want to do a min of this)
            mHeight += ((cImage*)newObject)->getSize().first;
            this->contents.push_back(std::make_pair(newObject, std::make_pair(std::string("__NONE__"), mHeight)));
            
            /* Synopsis */


            /* Create the texture first */
            mTexture = SDL_CreateTexture(
                    mRenderer,
                    SDL_PIXELFORMAT_UNKNOWN,
                    SDL_TEXTUREACCESS_TARGET,
                    w,
                    mHeight //Height is variable though so I had to pre-calc it =.=
                    );
            SDL_SetRenderTarget(mRenderer, mTexture);
            for(int i = 0; i < this->contents.size(); i++){
                this->contents[i].first->render(mRenderer);
            }

            /* Return the renderer to render to the window again */
            SDL_SetRenderTarget(mRenderer, NULL);

            /* Set default starting pos to 0, 0. Set here, because things will always be re-loaded */
            sauceRect.x = 0;
            sauceRect.y = 0;
        }
        catch(mException& e){
           setError(e.what()); 
           printf("%s \n", e.what());
        }
    }

    void cNovelDetails::move(int dx, int dy){ //DOES NOT SANITY CHECK!!! (Should be done beforehand anyway)
        /*Ignoring dx at the momemnt, until I can get zoom working. */
        sauceRect.y += dy;
        //sauceRect.x += dx;
    }

    void cNovelDetails::render(SDL_Renderer* mRenderer){ //TODO: Verify that this still works
        if(!loaded){
            std::string mError = currentDateTime() = ": ";
            mError += "[novelDetails.cpp] File wasn't loaded before calling render \n";
            printf("%s \n", mError.c_str());
            throw(mException(mError));
        }
        else{
            SDL_RenderCopy(mRenderer, mTexture, &sauceRect, NULL);
        }
    }

    void cNovelDetails::handleUserMouse(int x, int y, int button, bool isDown){
        return; //TODO
    }

    void cNovelDetails::handleUserKeyboard(std::string key, bool isDown, unsigned int modifiers){
        return; //TODO
    }

    void cNovelDetails::handleUserScroll(int dx, int dy){
        return; //TODO
    }

}

