/* It's finally here Kappa */
#include "contents.hpp"

namespace beatOff{

    cReader::cReader() : heightReached(0), mLog(new __logger::cLogger("log/details.log")) //I know this is bad code :(
    { }    

    cReader::cReader(__logger::cLogger* inl) : heightReached(0), mLog(inl)
    { }

    //TODO: Fix this config datastructure somehow.
    void cReader::loadChap(std::string fileLoc, SDL_Renderer* mRenderer, std::map<std::multimap<std::string, std::map<std::string, std::string> > >& mConfig)
    {
        try{
            pugi::xml_document doc ;
            pugi::xml_parse_result res = doc.load_file(sauce.c_str()); //Ignoring return value (I'm a NAUGHTY BOY elegiggle)
            if(!res){
                if(res.status == pugi::status_no_document_element || res.status == pugi::status_file_not_found){ //Empty document (i.e. the thing isn't downloaded yet)
                    this->loaded = 1;
                    throw(mException(std::string("[novelDetails.cpp] Warning: Document is empty! (perhaps the details have not been downlaoded yet?)")));
                }
                else throw(mException(std::string("[novelDetails.cpp] Error: Failed to open document pugiError: ") + res.description()));
            }
        } 
        catch(mException &e){
            setError(e.what());
            this->mLog->log(e.what());
        }
        catch(std::exception &e){
            setError(e.what());
            this->mLog->log(e.what());
        }
    }

}
