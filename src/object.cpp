#include "objects.hpp"

namespace beatOff{
    
    cObject::cObject() {}

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

    std::string getError(){
        return error;
    } 
    
    bool hasError(){
        return(error.size() > 0);
    }
}
