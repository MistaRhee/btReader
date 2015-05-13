#include <cstdio>
#include <cstring>
#include "btreader.hpp"

int main(int argc, char* argv[]){
    cMain mProg;
    if(argc > 1){
        /* They actually provided some arguments? Waht? Wai? No plz senpai stop*/
        if(!strcmp(argv[1], "-log")){
            freopen(argv[2], "w+", stdout);
        }
    }
    if(mProg.run()) return 0;
    else{
        printf("An error has occured in the program! BTReader Error: %s", mProg.getError().c_str());
        return -1;
    }
}
