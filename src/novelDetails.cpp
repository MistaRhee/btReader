#include "objects.hpp"

namespace beatOff{

    cNovelDetails::cNovelDetails(std::string config){
        configLoc = config;
        loadConfig();
    }

    cNovelDetails::cNovelDetails(SDL_Rect* inRect, std::string config){
        loaded = 0;
        selection = 0;
        setPos(inRect->x, inRect->y);
        setSize(inRect->h, inRect->w); //Like all things, height is variable, its just the width that stays the same
        mTexture = NULL;
        configLoc = config;
        loadConfig();
    }

    void cNovelDetails::openNovel(std::string sauce, SDL_Renderer* mRenderer, std::string font){
        try{
            /* Grabbing Volume + Chapter list */
            std::vector<std::vector<std::pair<std::string, std::string> > > volumes;
            XMLNode mainNode = XMLNode::openFileHelper(sauce.c_str(), "novel");
            title = mainNode.getChildNode("info").getAttribute("title");
            author = mainNode.getChildNode("info").getAttribute("author");
            synopsis = mainNode.getChildNode("synopsis").getText();

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
            SDL_SetRenderTarget(mRenderer, mTexture);

            int currentY = 0;
            cTextBox titleText(title.c_str(), font.c_str(), );
            
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
            printf("%s \n", mError);
            throw(mException(mError));
        }
        else{
            SDL_RenderCopy(mRenderer, mTexture, &sauceRect, NULL);
        }
    }

}

