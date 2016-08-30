#include "objects.hpp"

namespace beatOff{

    cButton::cButton() : cTextBox() {
        selected = 0;
        showBox();
    }

    cButton::cButton(std::string inText, std::string fontLoc, int inSize, int inX, int inY, int inW, int inH)
        : cTextBox(inText, fontLoc, inSize, inX, inY, inW, inH) {
        selected = 0;
        showBox();
    }
    void cButton::select(){
        if(!selected){
            invert();
            selected = 1;
        }
    }

    void cButton::deselect(){
        if(selected){
            invert();
            selected = 0;
        }
    }

}
