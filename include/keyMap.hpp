#ifndef KEYMAP_HPP
#define KEYMAP_HPP

#include <SDL2/SDL.h>
#include <map>
#include <string>

#include "mException.h"

namespace beatOff{

class cKeyMap{
    public:
        cKeyMap();
        ~cKeyMap();
        void addMapping(std::string, SDL_Keycode); //String to map the keycode to
        void editMapping(std::string, SDL_Keycode); //If there is no mapping, then it'll call add mapping
        bool exists(std::string); //CHecks if there is a mapping to an ID
        bool keyMapped(SDL_Keycode); //Checks if there is a mapping to a key
        SDL_Keycode getKey(std::string); //String to be queried
        const SDL_Keycode &getKey(std::string) const; //I think... =.=

    private:
        std::map<std::string, SDL_Keycode> mKeys;
};

}

#endif //KEYMAP_HPP
