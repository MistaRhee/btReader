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

            default:
                /* Not handling other events at the moment */
                break;
        }
    }
}

void cMain::handleUserKey(SDL_Keycode mKey, bool isDown, unsigned int modifiers){
    /* Just a stub at the moment */
    if(mKey == mKeys.getKey("up")){
        /* The key for up had an action */
        
    }
    else if(mKey == mKeys.getKey("down")){ //THe else to prevent double key spam
        
    }
    else{
        /* Unhandled key */
    }
}

void cMain::handleUserMouse(int x, int y, int button, bool isDown){
    /* Just a stub at the moment */
    switch(button){
        case SDL_BUTTON_LEFT:
            {
                /* Left mouse action */
                if(x >= contentLoc.x && x <= (contentLoc.x+contentLoc.w)){ //If the mouse's x position is in the range of the rect
                    if(y >= contentLoc.y && y <= (contentLoc.y+contentLoc.h)){ //If the mouse's y position is in the range of the rect
                        /* Send the event to the appropriate handler to update the program
                         * state */
                        switch(whereAt){
                            case settings:
                                {
                                    break;
                                }

                            case showNovels:
                                {
                                    break;
                                }

                            case novelDetails:
                                {
                                    break;
                                }

                            case reader:
                                {
                                    break;
                                }

                            case dlList:
                                {
                                    break;
                                }

                            default:
                                {
                                    std::string mError = currentDateTime() + ": ";
                                    mError += "[events.cpp] Error! Invalid where at found in handleUserScroll";
                                    printf("%s \n", mError.c_str());
                                    setError(mError);
                                    break;
                                }
                        }
                    }

                }

                break;
            }

        case SDL_BUTTON_RIGHT:
            {
                /* Right mouse action */

                break;
            }

        case -1: //If no mouse button down, just movement
            {
                break;
            }

        default:
            {
                /* Unhandled mouse events */
                break;
            }
    }
    
}

void cMain::handleUserScroll(int dx, int dy){
    int mx = -1, my = -1;
    SDL_GetMouseState(&mx, &my);
    if(mx >= contentLoc.x && mx <= (contentLoc.x+contentLoc.w)){ //If the mouse is within the width of content rect
        if(my >= contentLoc.y && my <= (contentLoc.y+contentLoc.h)){ //If the mouse is actually within the rectangle of contents
            /* Send the event to the appropriate handler to update the program
             * state */
            switch(whereAt){
                case novelDetails:
                    break;

                case settings:
                    break;

                case showNovels:
                    break;

                case reader:
                    break;

                case dlList:
                    break;

                default:
                    std::string mError = currentDateTime() + ": ";
                    mError += "[events.cpp] Error! Invalid where at found in handleUserScroll";
                    printf("%s \n", mError.c_str());
                    setError(mError);
                    break;
            }
        }
        
    }
}
