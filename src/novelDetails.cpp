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

    void cNovelDetails::openNovel(std::string sauce, SDL_Renderer* mRenderer, std::string font){
        try{
            /* Clearing out old textures */
            if(mTexture){
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
            }

            /* Grabbing Volume + Chapter list */
            std::vector<std::vector<std::pair<std::string, std::string> > > volumes;
            XMLNode mainNode = XMLNode::openFileHelper(sauce.c_str(), "novel");
            std::string title = mainNode.getChildNode("info").getAttribute("title");
            std::string author = mainNode.getChildNode("info").getAttribute("author");
            std::string synopsis = mainNode.getChildNode("synopsis").getText();

            /* Grab the image location to render 
             * This only grabs the cover image from the first volume. I could
             * edit it so that it would include the first cover image that
             * exists, but that is for later ~~~~~~~~~~~~~
             * ***************************************************************/
            std::string frontLoc = mainNode.getChildNode("volume", 0).getAttribute("image"); 

            for(int i = 0, j = mainNode.nChildNode("volume"); i < j; i++){
                XMLNode volume = mainNode.getChildNode("volume", i);
                std::vector<std::pair<std::string, std::string> > chapterDetails;
                for(int k = 0, l = volume.nChildNode("chapter"); k < l; k++){
                    XMLNode chapNode = volume.getChildNode("chapter", i);
                    chapterDetails.push_back(std::make_pair(chapNode.getAttribute("title"), chapNode.getAttribute("location"))); //Wew!
                }
                volumes.push_back(chapterDetails);
            }
            loaded = 1;
            
            /* TIME TO RENDER TO A TEXTURE !!! */

            /* Calculate the height beforehand, because texture height must be
             * declared beforehand */

            cTextBox tempBox; //A temporary textbox that will only be used for the "wrapped height" method
            tempBox.setFont(font); //Font will always be here
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

    void cNovelDetails::move(int dx, int dy){
        /*Ignoring dx at the momemnt, until I can get zoom working. */
        sauceRect.y += dy;
        //sauceRect.x += dx;
    }

    void cNovelDetails::render(SDL_Renderer* mRenderer){
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

}

