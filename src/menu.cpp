#include "contents.hpp"

namespace beatOff{

    cMenu::cMenu(){
        this->next = list;
        this->mouseMoved = 0;
    }

    void cMenu::render(SDL_Renderer* mRenderer){ //TODO
        return;
    }

    void cMenu::handleUserMouse(int x, int y, int button, bool isPressed){ //TODO
        switch(button){
            case SDL_BUTTON_LEFT:
                break;

            case SDL_BUTTON_RIGHT:
                break;

            case SDL_BUTTON_MIDDLE:
                break;

            case -1:
                //Mouse motion
                break;

            default:
                //Not handling this
                break;
        }
    }

    void cMenu::handleUserKeyboard(std::string key, bool isDown, unsigned int modifiers){ //TODO
        return;
    }
}
