#include <cstdio>
#include "reader.hpp"

int main(){
    cReader mReader;
    if(mReader.run()) return 0;
    else return mReader.getError();
}
