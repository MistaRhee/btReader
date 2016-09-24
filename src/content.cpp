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

    void cContent::setRect(SDL_Rect inRect){
        sRect = inRect; //The "viewport" of the texture
        sRect.x = 0;
        sRect.y = 0;
        setPos(inRect.x, inRect.y); //The position to render to on screen
        /* Like all things, height is variable, its just the width that stays the same */
        setSize(inRect.h, inRect.w); 
    }
}
