#include "objects.hpp"

namespace beatOff{

    cObject::cObject(){
        /* Uninitialise all variables in an object */
        this->x = -1;
        this->y = -1;
        this->h = -1;
        this->w = -1;
    }

    void cObject::setPos(int inX, int inY){
        x = inX;
        y = inY;
    }

    void cObject::setSize(int inH, int inW){
        h = inH;
        w = inW;
    }

    void cObject::move(int dx, int dy){
        x += dx;
        y += dy;
    }

    void cObject::setError(std::string in){
        error += in + "\n";
    }

    std::string cObject::getError(){
        return error;
    } 

    bool cObject::hasError(){
        return(error.size() > 0);
    }

    void cObject::setWarning(std::string in){
        warning += in + "\n";
    }

    std::string cObject::getWarning(){
        std::string rVal = warning;
        warning.clear();
        return rVal;
    }

    bool cObject::hasWarning(){
        return (warning.size() > 0);
    }

    bool cObject::isOver(int x, int y){
        bool rVal = 1;
        //Mouse is left of the button
        if(x < this->x){
            rVal = false;
        }
        //Mouse is right of the button
        else if(x > this->x + this->w){
            rVal = false;
        }
        //Mouse above the button
        else if(y < this->y){
            rVal = false;
        }
        //Mouse below the button
        else if(y > this->y + this->h){
            rVal = false;
        }
        return rVal;
    }
}
