#ifndef KEYMAP_HPP
#define KEYMAP_HPP

#include <SDL2/SDL.h>

#include <map>
#include <string>

#include "mException.h"

namespace beatOff{

/* Can have multiple keys doing the same thing, can't have one key doing
 * multiple things
 * 
 * To have one key do multiple things (NOT RECOMMENDED), flip the keycode and
 * string and then rewrite everything, because I'm not going to do that.
 */
class cKeyMap{
    public:
        cKeyMap();
        ~cKeyMap();
        void addMapping(SDL_Keycode, std::string); //Mapping key to a string. If already exists, throw error
        void editMapping(SDL_Keycode, std::string); //Mapping key to a string. If key doesn't already exist, throw error
        bool exists(SDL_Keycode); //Checks if there is a mapping to an ID
        bool mappingExists(std::string); //Checks if there is a mapping to a key
        std::string getKey(SDL_Keycode); //String to be queried
        const std::string& getKey(SDL_Keycode) const; //Don't think no more

    private:
        std::map<SDL_Keycode, std::string> mKeys; //Maps a key to a nice name to use
};

}

#endif //KEYMAP_HPP
