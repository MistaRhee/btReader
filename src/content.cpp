#include "contents.hpp"

/* Note to self:
 * When creating the texture, ensure that the height is recalculated. Width is
 * fine, but not height
 * **************************************************************************/

namespace beatOff{

    cContent::cContent(){
        this->state = working;
        this->err.clear();
    }

    void cContent::setFont(std::string in){
        fontLoc = in;
    }

    void cContent::setFontSize(int in){
        fontSize = in;
    }

    void cContent::setTextColour(SDL_Color in){
        textColour = in;
    }

    void cContent::setBackColour(SDL_Color in){
        backColour = in;
    }

}
