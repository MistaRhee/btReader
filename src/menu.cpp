#include "contents.hpp"

namespace beatOff{

    void cMenu::render(SDL_Renderer* mRenderer){
        return;
    }

    void cMenu::handleUserMouse(int x, int y, int button, bool isPressed){
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

}