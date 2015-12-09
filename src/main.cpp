#include <cstdio>
#include <cstring>

#include "btReader.hpp"

int main(int argc, char* argv[]){
    cMain mProg;
    if(argc > 1){
        /* They actually provided some arguments? Waht? Wai? No plz senpai stop*/
        if(!strcmp(argv[1], "-log")){
            freopen(argv[2], "w+", stdout);
        }
    }
    if(mProg.run()){
        delete(&mProg);
        return 0;
    }
    else{
        fprintf(stderr, "An error had occured. Please look at the log files for more details \n");
        delete(&mProg);
        return -1;
    }
}
