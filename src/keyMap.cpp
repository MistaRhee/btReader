#include "keyMap.hpp"

namespace beatOff{

cKeyMap::cKeyMap(){
    /* Nothing here ATM */
}

cKeyMap::~cKeyMap(){
    /* Still nothing to see here */
}

void cKeyMap::addMapping(std::string name, SDL_Keycode key){ 
    /* Assuming there is no existing key mapping. If there is, it'll overwrite
     * */
    if(!mKeys.count(name))
        mKeys.insert(std::make_pair(name, key));
    
    else
        mKeys[name] = key;
}

void cKeyMap::editMapping(std::string name, SDL_Keycode newKey){
    /* This assumes that there is no previous mapping for the SDL_Keycode. Will
     * throw an exception if name doesn't exist */
    if(mKeys.count(name))
        mKeys.insert(std::make_pair(name, newKey));
    
    else
        throw(mException("[keyMap.cpp] - Error! name for editMapping doesn't exist \n"));
    
}

SDL_Keycode& cKeyMap::getKey(std::string name){
    if(!mKeys.count(name))
        throw(mException("[keyMap.cpp] - Error! Name that was requested doesn't exist! \n"));
    
    auto found = mKeys.find(name);
    return found->second;
}

const SDL_Keycode& cKeyMap::getKey(std::string name) const{
    if(!mKeys.count(name))
        throw(mException("[keyMap.cpp] - Error! Name that was requested doesn't exist! \n"));
    
    auto found = mKeys.find(name);
    return found->second;
}

bool cKeyMap::exists(std::string id){
    bool rVal = 0;
    if(mKeys.count(id))
        rVal = 1;
    
    return rVal;
}

bool cKeyMap::keyMapped(SDL_Keycode mKey){ //ONLY COUNTS ONE INSTANCE!!!
    bool rVal = 0;
    for(auto it = mKeys.begin(); it != mKeys.end(); ++it){
        if(it->second == mKey){
            rVal = 1;
            break;
        }
    }
    return rVal;
}

}
