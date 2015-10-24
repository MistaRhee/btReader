#include "btReader.hpp"

void cMain::processEvents(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_KEYDOWN:
                handleUserKey(
                        e.key.keysym.sym, 
                        e.key.type==SDL_KEYDOWN,
                        e.key.keysym.mod
                        );
                break;
                
            case SDL_KEYUP:
                handleUserKey(
                        e.key.keysym.sym,
                        e.key.type==SDL_KEYDOWN,
                        e.key.keysym.mod
                        );
                break;

            case SDL_MOUSEBUTTONDOWN:
                handleUserMouse(
                        e.button.x, 
                        e.button.y, 
                        e.button.button, 
                        e.button.type==SDL_MOUSEBUTTONDOWN
                        );
                break;

            case SDL_MOUSEBUTTONUP:
                handleUserMouse(
                        e.button.x,
                        e.button.y,
                        e.button.button,
                        e.button.type==SDL_MOUSEBUTTONDOWN
                        );
                break;

            case SDL_MOUSEWHEEL:
                handleUserScroll(
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

            case SDL_QUIT:
                commitSudoku(); //I'm sorry I failed you QQ
                break;
                
            default:
                /* Not handling other events at the moment */
                break;
        }
    }
}

void cMain::handleUserKey(SDL_Keycode mKey, bool isDown, unsigned int modifiers){
    /* Sends the correct token to the appropriate content object */
    for(auto it = mContents.begin(); it != mContents.end(); ++it){
        if(it->second->isFocused()){
            
        }
    }
}

void cMain::handleUserMouse(int x, int y, int button, bool isDown){
    /* Check which "content" the user's mouse is over */
    for(auto it = mContents.begin(); it != mContents.end(); ++it){
        if(it->second->isOver(x, y) && it->second->isInUse()){
            /* Send the signal to the appropriate content */
            it->second->handleUserMouse(x, y, button, isDown);
            it->second->inFocus();
            break; //No need to check any more
        }
    }
}

/* Scrolling currently doesn't change focus. MIGHT change if it becomes an
 * issue later
 */
void cMain::handleUserScroll(int dx, int dy){
    int mx = -1, my = -1;
    SDL_GetMouseState(&mx, &my);
    for(auto it = mContents.begin(); it != mContents.end(); ++it){
        if(it->second->isOver(x, y) && it->second->isInUse()){
            it->second->handleUserScroll(dx, dy);
            break;
        }
    }
}
