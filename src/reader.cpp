#include "objects.hpp"

namespace beatOff{

    cReader::cReader(SDL_Rect* inRect){
       loaded = 0;
       setPos(inRect->x, inRect->y);
       setSize(inRect->h, inRect->w); //Height will be varied anyway
    }

    void cReader::getContent(std::string sauce){
        FILE*fin = fopen(sauce.c_str(), "r");
    }

}
