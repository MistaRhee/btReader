#include "contents.hpp"

namespace beatOff{

    cNovelDetails::cNovelDetails(){
        loaded = 0;
        selection = 0;
        mTexture = NULL;
    }

    void cNovelDetails::setRect(SDL_Rect inRect){
        sauceRect = inRect; //The "viewport" of the texture
        setPos(inRect.x, inRect.y); //The position to render to on screen
        /* Like all things, height is variable, its just the width that stays the same */
        setSize(inRect.h, inRect.w); 
    }

    void cNovelDetails::openNovel(std::string sauce, SDL_Renderer* mRenderer, std::string fontLoc){ //TODO: WIP. Just need to pre-calculate the height then render it to a texture
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
            std::string frontLoc = mainNode.child("volume").attribute("image").value();

            for(auto currNode: mainNode.children("volume")){
                std::vector<std::pair<std::string, std::string> > chapterDetails;
                for(auto chapterNode: currNode.children("chapter")){
                    chapterDetails.push_back(std::make_pair(chapterNode.attribute("title").value(), chapterNode.attribute("location").value())); //Wew!
                }
                volumes.push_back(chapterDetails);
            }
            loaded = 1;
            
            /* TIME TO RENDER TO A TEXTURE !!! */

            /* Calculate the height beforehand, because texture height must be
             * declared beforehand */

            cTextBox tempBox; //A temporary textbox that will only be used for the "wrapped height" method
            tempBox.setFont(fontLoc); //Font will always be here
            int mHeight = 0;
            tempBox.setText(title);
//            tempBox.setTextSize();

            /* Create the texture first */
            mTexture = SDL_CreateTexture(
                    mRenderer,
                    SDL_PIXELFORMAT_UNKNOWN,
                    SDL_TEXTUREACCESS_TARGET,
                    w,
                    mHeight //Height is variable though so I had to pre-calc it =.=
                    );
            SDL_SetRenderTarget(mRenderer, mTexture);
/*
            int currentY = 0;
            cTextBox titleText(title.c_str(), font.c_str(), );
*/            
            /* Return the renderer to render to the window again */
            SDL_SetRenderTarget(mRenderer, NULL);

            /* Set default starting pos to 0, 0 */
            sauceRect.x = 0;
            sauceRect.y = 0;
            /* The h and w of sauceRect SHOULD've been set in the constructor. */

        }
        catch(mException& e){
           setError(e.what()); 
           printf("%s \n", e.what());
        }
    }

    void cNovelDetails::move(int dx, int dy){ //TODO: Verify that this still works
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

