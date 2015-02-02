#include "btreader.hpp"

cMain::cMain(){
    preComp();
}

void cMain::preComp(){
    if(!fileExists()){
        printf("No database found! Rebuilding novel list from site! \n");
        createDatabase();
    }
    else readDatabase();
}
