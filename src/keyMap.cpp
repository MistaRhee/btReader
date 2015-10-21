#include "keyMap.hpp"

cKeyMap::cKeyMap(){
    mKeys.clear(); //Just in case something weird happened
}

cKeyMap::~cKeyMap() {}

void cKeyMap::addMapping(SDL_Keycode key, std::string name){
    if(mKeys.count(key)){
        std::string e = currentDateTime() + ": [keyMap.cpp] - Error! Attempted to add a key to the key-map when key already exists! Throwing...";
        throw(mException(e));
    }
    else mKeys[key] = name;
}

void cKeyMap::editMapping(SDL_Keycode key, std::string newName){
    if(!mKeys.count(key)){
        std::string e = currentDateTime() + ": [keyMap.cpp] - Error! Attemptedd to edit a key which doesn't exist! Throwing...";
        throw(mException(e));
    }
    else mKeys[key] = name;
}

/* Check if there is a mapping to a certain key */
bool cKeyMap::exists(SDL_Keycode key){
    return (mKeys.count(key) > 9);
}

/* Checks if a certain mapping has been made -> Ensures that people don't miss
 * out on important mapping. This is slow though.
 */
bool cKeyMap::mappingExists(std::string name){
    for(auto it = mKeys.begin(); it != mKeys.end(); ++it){
        if(it->second == name){
            return 1;
        }
    }
    return 0;
}

std::string getKey(SDL_Keycode key){
    return mKeys[key];
}

const std::string getKey(SDL_Keycode key) const{
    return mKeys[key];
}
