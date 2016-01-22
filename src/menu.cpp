#include "contents.hpp"

namespace beatOff{

    cMenu::cMenu(){
        this->next = list;
        this->mouseMoved = 0;
        this->mLog = new __logger::cLogger("logs/menu.log");
    }

    cMenu::cMenu(__logger::cLogger* mLog){
        this->next = list;
        this->mouseMoved = 0;
        this->mLog = mLog;
    }

    void cMenu::render(SDL_Renderer* mRenderer){ 
        for(auto it = images.begin(); it != images.end(); ++it){
            it->second.render(mRenderer);
        }
        for(auto it = buttons.begin(); it != buttons.end(); ++it){
            it->second.render(mRenderer);
        }
    }

    void cMenu::handleUserMouse(int x, int y, int button, bool isPressed){
        if(isPressed){ //Currently not handling hold, move off and release kind of things
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
                            if(name == "settings"){
                                this->state = go;
                                this->next = settings;
                            }
                            else if(name == "novelList"){
                                this->state = go;
                                this->next = list;
                            }
                            else if(name == "downloads"){
                                this->state = go;
                                this->next = dlList;
                            }
                            else{
                                this->mLog->log(std::string("[menu.cpp] Error: Registered a button click, but there is an unrecognised object ") + name);
                            }
                        }
                        else{
                            /* We have an issue right now */
                            this->mLog->log(std::string("[menu.cpp] Error: Handle usermouse was called but the mouse was not over any object! Skipping that event (not critical)! "));
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
    }

    void cMenu::handleUserKeyboard(std::string key, bool isDown, unsigned int modifiers){ //TODO
        /* Keyboard doesn't register for menu, only for the content below */
        return;
    }

    void cMenu::handleUserScroll(int dx, int dy){
        /* Menu doesn't handle scrolling for obvious reasons */
        return;
    }

    void cMenu::addButton(std::string name, std::string text, std::string font, int size, int x, int y, int h, int w){
        beatOff::cButton newButt(text, font, size, x, y, w, h);
        newButt.centre();
        this->buttons[name] = newButt;
    }

    void cMenu::addImage(std::string name, std::string sauce, int x, int y, int h, int w){
        beatOff::cImage newIma(sauce, x, y, h, w);
        this->images[name] = newIma;
    }

    void cMenu::changeImage(std::string name, std::string sauce){
        /* Only changes something if it exists */
        if(name == "downloads"){
            if(this->images.count(name)){
                std::unique_lock<std::mutex> ul(this->accessible, std::try_to_lock);
                if(!ul.owns_lock()){
                    this->mLog->log("[menu.cpp] Info: Attempted to change updates are in progress, ignoring"); //TODO fix when DLList page is actually implemented
                }
                else{
                    this->images[name].setPicLoc(sauce);
                }
            }
        }
        else if(this->images.count(name)) this->images[name].setPicLoc(sauce);
    }

    void cMenu::selectButton(std::string name){
        this->buttons[name].select();
    }

    places_t cMenu::getSelected(){
        return this->next;
    }
}
