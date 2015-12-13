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

    /* Either the menu is in focus or the actual element is in focus */

    if(mContents[whereAt]->isInFocus()){
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
    else{ //It was meant for the menu
        /* Sanity check anyway just in case I forgot an edge case! */
        if(!mContents[menu]->isInFocus()){
            this->mLog->log("[events.cpp] Error: Neither whereAt nor menu was in focus! Aborting!");
            setError();
        }
        else{ //Satisfies sanity check, throw it to the appropraite handler
            ((beatOff::cMenu*)mContents[menu])->handleUserKeyboard(mKeys.getKey(mKey), isDown, modifiers);
        }
    }
}

void cMain::handleUserMouse(int x, int y, int button, bool isDown){
    /* Check which "content" the user's mouse is over */
    /* It is either the menu or the actual content */
    if(!mContents[menu]->isOver(x, y)){ //Check menu first because it's always rendered "on top" of the content (although they should be in separate regions)
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
    int mx = -1, my = -1;
    SDL_GetMouseState(&mx, &my);
    for(auto it = mContents.begin(); it != mContents.end(); ++it){
        if(it->second->isOver(mx, my) && it->second->isInUse()){
            it->second->handleUserScroll(dx, dy);
            break; //No need to check any more
        }
    }
}
