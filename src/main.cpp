#include <cstdio>
#include "btreader.hpp"

int main(int, char**){
    cMain mProg;
    if(mProg.run()) return 0;
    else{
        printf("An error has occured in the program! BTReader Error: %s", mProg.getError().c_str());
        return -1;
    }
}
