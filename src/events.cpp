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

void cMain::handleUserMouse(int x, int y, unsigned int button, bool isDown){
    /* Just a stub at the moment */
    switch(button){
        case SDL_BUTTON_LEFT:
            //Left mouse action
            break;
        default:
            break;
    }
    
}

void cMain::handleUserScroll(int dx, int dy){
    int mx = -1, my = -1;
    SDL_GetMouseState(&x, &y);

}
