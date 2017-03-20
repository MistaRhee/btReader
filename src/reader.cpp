/* It's finally here Kappa */
#include "contents.hpp"

namespace beatOff{

    cReader::cReader() : heightReached(0), mLog(new __logger::cLogger("log/details.log")) //I know this is bad code :(
    { }    

    cReader::cReader(__logger::cLogger* inl) : heightReached(0), mLog(inl)
    { }

    //TODO: Fix this config datastructure somehow.
    void cReader::loadChap(std::string fileLoc, SDL_Renderer* mRenderer, std::map<std::multimap<std::string, std::map<std::string, std::string> > >* mConfig)
    {
        
    }

}
