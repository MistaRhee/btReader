#include <cstdio>
#include "swtich.hpp"

int main(){
    cSwitch mProg;
    if(mProg.run()) return 0;
    else return mProg.getError();
}
