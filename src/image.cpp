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
            mTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
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
        SDL_FreeSurface(mSurface);
        SDL_DestroyTexture(mTexture);
    }
    
}
