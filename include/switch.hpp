#ifndef SWITCH_HPP
#define SWITCH_HPP


#include <curl/curl.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <vector>
#include <utility>
#include <map>

#include "menu.hpp"
#include "reader.hpp"

enum state_t{
    read,
    grab,
    parse
}

class cSwitch{ //Switchboard between the different states that the reader can be in ("Reader" state, "Grabbing" state and the "Parsing" state). No I will not be multithreading. You want that, you do it yourself asshole.
    public:
        void run();
        void update();
        int getError(); //Kinda redundant, but it will display the precise area for known POSSIBLE errors (there will be popups anyway so w/e)
    private:
        state_t state;
        cMenu mMenu;
        cReader mReader;
        int error;

};

#endif //SWITCH_HPP
