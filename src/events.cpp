#include "btreader.hpp"

void cMain::processEvents(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_KEYBOARD:
                handleUserKey(
                        e.key.keysym.sym, 
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

            case SDL_MOUSEWHEEL:
                handleScroll(
                        e.wheel.x,
                        e.wheel.y
                        );
                break;

            case SDL_MOUSEMOTION:
                handleUserMouse(
                        e.motion.x,
                        e.motion.y,
                        -1, //Uninitialized mouse button
                        0 //Ignored anyway because no button is found
                        );
                break;

            default:
                /* Not handling other events at the moment */
                break;
        }
    }
}

void cMain::handleUserKey(SDL_Keycode mKey, bool isDown, unsigned int modifiers){
    /* Just a stub at the moment */
    switch(mKey){
        case keyMapping["up"]:
            //The key for up had an action
            break;

        case keyMapping["down"]:
            break;

        default:
            break;
    }
}

void cMain::handleUserMouse(int x, int y, int button, bool isDown){
    /* Just a stub at the moment */
    switch(button){
        case SDL_BUTTON_LEFT:
            /* Left mouse action */
            if(x >= contentLoc.x && x <= (contentLoc.x+contentLoc.w)){ //If the mouse's x position is in the range of the rect
                if(y >= contentLoc.y && y <= (contentLoc.y+contentLoc.h)){ //If the mouse's y position is in the range of the rect
                    
                }
                
            }

            break;

        case -1: //If no mouse button down, just movement
            break;

        default:
            break;
    }
    
}

void cMain::handleUserScroll(int dx, int dy){
    int mx = -1, my = -1;
    SDL_GetMouseState(&x, &y);

}
