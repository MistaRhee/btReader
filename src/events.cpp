#include "btreader.hpp"

void cMain::processEvents(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_KEYBOARD:
                handleUserKey(
                        e.key.keysym, 
                        e.key.type==SDL_KEYDOWN
                        );
                break;
            case SDL_MOUSEBUTTON:
                handleUserMouse(
                        e.button.x, 
                        e.button.y, 
                        e.button.button, 
                        e.button.type==SDL_MOUSEBUTTONDOWN
                        );
                break;

            default:
                /* Not handling other events at the moment */
                break;
        }
    }
}
