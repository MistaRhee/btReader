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

    /* Only the content recieves keyboard signals */

    switch(whereAt){
        case list:
            ((beatOff::cNovelDetails*)mContents[list])->handleUserKeyboard(mKeys.getKey(mKey), isDown, modifiers);
            break;

        case details:
            ((beatOff::cNovelDetails*)mContents[details])->handleUserKeyboard(mKeys.getKey(mKey), isDown, modifiers);
            break;

        case settings:
            /* Not ready yet */
            //TODO on completion of settings menu
            break;

        case dlList:
            /* Not ready yet */
            //TODO on completion of dlList menu
            break;

        default:
            this->mLog->log("[events.cpp] Error: Entered invalid whereAt state during handling user keyboard! Aborting!");
            setError();
            break;
    }
}

void cMain::handleUserMouse(int x, int y, int button, bool isDown){
    /* Check which "content" the user's mouse is over */
    /* It is either the menu or the actual content */
    if(mContents[menu]->isOver(x, y)){ //Check menu first because it's always rendered "on top" of the content (although they should be in separate regions)
        ((beatOff::cMenu*)mContents[menu])->handleUserMouse(x, y, button, isDown);
    }
    else{
        switch(whereAt){
            case list:
                ((beatOff::cNovelList*)mContents[list])->handleUserMouse(x, y, button, isDown);
                break;

            case details:
                ((beatOff::cNovelDetails*)mContents[details])->handleUserMouse(x, y, button, isDown);
                break;

            case settings:
                //TODO on completion of settings menu
                break;

            case dlList:
                //TODO on completion of DLList
                break;

            default:
                this->mLog->log("[events.cpp] Error: Entered invalid whereAt state during handling user mouse! Aborting!");
                setError();
                break;

        }
    }
}

/* Scrolling currently doesn't change focus. MIGHT change if it becomes an
 * issue later
 */
void cMain::handleUserScroll(int dx, int dy){
    /* Check which "content" the user's mouse is over */
    /* It is either the menu or the actual content */
    int mx = -1, my = -1;
    SDL_GetMouseState(&mx, &my);
    /* Might want to include a sanity-check on mx and my (then again already accounted for below */
    if(!mContents[menu]->isOver(mx, my)){ //Check menu first because it's always rendered "on top" of the content (although they should be in separate regions)
        ((beatOff::cMenu*)mContents[menu])->handleUserScroll(dx, dy);
    }
    else if(mContents[whereAt]->isOver(mx, my)){
        switch(whereAt){
            case list:
                ((beatOff::cNovelList*)mContents[list])->handleUserScroll(dx, dy);
                break;

            case details:
                ((beatOff::cNovelDetails*)mContents[details])->handleUserScroll(dx, dy);
                break;

            case settings:
                //TODO on completion of settings menu
                break;

            case dlList:
                //TODO on completion of DLList
                break;

            default:
                this->mLog->log("[events.cpp] Error: Entered invalid whereAt state during handling user Scroll! Aborting!");
                setError();
                break;

        }
    }
    else{
        /* Mouse action off the screen, log it and then do nothing */
        this->mLog->log(std::string("[events.cpp] Info: Recieved mouse event off the screen with coordinates: x->")+
                std::to_string(mx) +
                std::string(" y->") + std::to_string(my)
                ); //Wew
    }
}
