#include "contents.hpp"

/* Note to self:
 * When creating the texture, ensure that the height is recalculated. Width is
 * fine, but not height
 * **************************************************************************/

namespace beatOff{

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

    bool cContent::isOver(int x, int y){
        /* Resolving conflicts by dropping the bottom line and the very far
         * right edge -> If it becomes an issue, we can just switch to the next
         * a less hacky way of doing this, but ATM, it's fine
         */
        return(((x >= this->x)&(x < this->x + this->w)&(y >= this->y)&(y < this->y + this->h)));
    }

    void cContent::inFocus(){
        focus = 1;
    }

    void cContent::offFocus(){
        focus = 0;
    }

    bool cContent::isInFocus(){ return focus; }
}
