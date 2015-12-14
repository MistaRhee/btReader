#include "contents.hpp"

namespace beatOff{

    cMenu::cMenu(){
        this->next = list;
        this->mouseMoved = 0;
    }

    void cMenu::render(SDL_Renderer* mRenderer){ 
        for(auto it = images.begin(); it != images.end(); ++it){
            it->second.render(mRenderer);
        }
        for(auto it = buttons.begin(); it != buttons.end(); ++it){
            it->second.render(mRenderer);
        }
        return;
    }

    void cMenu::handleUserMouse(int x, int y, int button, bool isPressed){ //TODO
        switch(button){
            case SDL_BUTTON_LEFT:
                {
                    std::string name;
                    /* Iterate through all the buttons and images, see what was selected */
                    for(auto it = images.begin(); it != images.end(); ++it){
                        if(it->second.isOver(x, y)){
                            name = it->first;
                            break;
                        }
                    }
                    if(name.empty()){
                        for(auto it = buttons.begin(); it != buttons.end(); ++it){
                            if(it->second.isOver(x, y)){
                                name = it->first;
                                break;
                            }
                        }
                    }
                    if(!name.empty()){
                        /* Check what the name is, and then set what we're going to next */
                        if(name == ""){

                        }
                        else{
                            this->state = stuck;
                            this->err = std::string("[menu.cpp] Error: Registered a button click, but there is an unrecognised object ") + name;
                        }
                    }
                    else{
                        /* We have an issue right now */
                        this->state = stuck;
                        this->err = std::string("[menu.cpp] Error: Handle usermouse was called but the mouse was not over any object! Skipping that event (not critical)!");
                    }
                    break;
                }

            case SDL_BUTTON_RIGHT:
                /* We don't handle right click at this stage (no need to) */
                break;

            case SDL_BUTTON_MIDDLE:
                /* Don't handle middle-mouse at all */
                break;

            case -1:
                //Mouse motion
                /* Do nothing, since we don't highlight what the user is hovering over in the menu */
                break;

            default:
                //Not handling this
                break;
        }
    }

    void cMenu::handleUserKeyboard(std::string key, bool isDown, unsigned int modifiers){ //TODO
        /* Keyboard doesn't register for menu, only for the content below */
        return;
    }

    void cMenu::handleUserScroll(int dx, int dy){
        /* Menu doesn't handle scrolling for obvious reasons */
        return;
    }
}
