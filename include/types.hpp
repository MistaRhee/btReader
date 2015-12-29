#ifndef TYPES_HPP
#define TYPES_HPP

enum places_t{
    menu = -1, //Should actually have to maually call this
    none, //Doesn't go anywhere (so the errornous thing)
    list, //NovelList
    settings, //Settings
    details, //Novel Details
    dlList //List of DLs
    //updates (maybe implementing later)
};

enum state_t{ //The state of a content object
    broken, //It has an error that has broken the program
    go, //Is ready for custom action
    working //It is currently working as intended
};


#endif //TYPES_HPP
