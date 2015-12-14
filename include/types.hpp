#ifndef TYPES_HPP
#define TYPES_HPP

enum places_t{
    menu = -1, //Should actually have to maually call this
    list, //NovelList
    settings, //Settings
    details, //Novel Details
    dlList //List of DLs
    //updates (maybe implementing later)
};

enum state_t{ //The state of a content object
    stuck, //It has an error that needs to be logged
    broken, //It has an error that has broken the program
    rendering, //Is being displayed on screen
    go, //Is ready for custom action
    hidden //Isn't being rendered
};


#endif //TYPES_HPP
