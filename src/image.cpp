#include "objects.hpp"

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

namespace beatOff{

    cImage::cImage(){
        if(IMG_Init(IMG_INIT_JPG||IMG_INIT_PNG) < 0){
            std::string e = "cImage error: SDL_Image failed to init (Image Error: " + IMG_GetError() + ")";
            printf ("%s", e.c_str());
            setError(e);
        }
        setSize(-1, -1);
    }

    cImage::cImage(std::string inLoc, int inx, int iny, int inh, int inw){ 
        if(IMG_Init(IMG_INIT_JPG||IMG_INIT_PNG) < 0){
            std::string e = "cImage error: SDL_Image failed to init (Image Error: " + IMG_GetError() + ")";
            printf ("%s", e.c_str());
            setError(e);
        }
        setPicLoc(inLoc);
        setPos(inx, iny);
        setSize(inh, inw);
    }

    void cImage::setPicLoc(std::string inLoc){
        location = inLoc;
    }

    std::pair<int, int> getSize(){
        std::pair<int, int> rVal;
        if(!fileExists(location)){
            printf("cImage Warning! No image set before checking size! \n");
            rVal = std::make_pair(-1, -1);
        }
        else{
            SDL_Surface* mSurface = NULL;
            mSurface = IMG_Load(location.c_str());
            if(mSurface == NULL){
                printf("cImage Error! Failed to load image \n");
                std::string e = "cImage Error - Image failed to load (SDL_Image error: " + IMG_GetError() + ")";
                setError(e);
                rVal = std::make_pair(-1, -1);
            }
            else{
                rVal = std::make_pair(mSurface->h, mSurface->w);
                SDL_FreeSurface(mSurface);
            }
        }
        return rVal;
    }

    void cImage::render(SDL_Renderer* mRenderer){
        SDL_Surface* mSurface = NULL;
        SDL_Texture* mTexture = NULL;
        SDL_Rect dRect;
        if(!fileExists(location)){
            printf("cImage Error! No image found! \n");
            std::string e = "cImage Error - No image found (Image location: " + location + ")";
            setError(e)
        }
        else{
            mSurface = IMG_Load(location.c_str());
            if(!mSurface or mSurface == NULL){
                printf("cImage Error! Failed to load image \n");
                std::string e = "cImage Error - Image loading failure (SDL_Image error: " + IMG_GetError() + ")";
                setError(e);
            }
            else{
                mTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
                if(mTexture == NULL){
                    printf("cImage Error! Failed to create texture \n");
                    std::string e = "cImage Error - Failed to create texture from surface (SDL Error: " + SDL_GetError() + ")";
                    setError(e);
                }
                else{
                    dRect.x = x;
                    dRect.y = y;
                    if(h < 0 or w < 0){
                        dRect.h = mSurface->h;
                        dRect.w = mSurface->w;
                    }
                    else{
                        dRect.h = h;
                        dRect.w = w;
                    }
                    SDL_RenderCopy(mRenderer, mTexture, NULL, &dRect);
                }
            }
        }
        SDL_FreeSurface(mSurface);
        SDL_DestroyTexture(mTexture);
    }
    
}
