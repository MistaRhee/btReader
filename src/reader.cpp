/* It's finally here Kappa */
#include "contents.hpp"

#define R_FONT_LOOKUP(...) (mConfig)["fontList"].find(__VA_ARGS__)->second["sauce"]

namespace beatOff{

    cReader::cReader()
    {
		this->mLog = new __logger::cLogger("log/details.log");
	}    

    cReader::cReader(__logger::cLogger* inl)
    { 
		this->mLog = inl;
	}

    void cReader::loadChap(std::string sauce, SDL_Renderer* mRenderer, std::map<std::string, std::multimap<std::string, std::map<std::string, std::string> > >& mConfig)
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
			
			unsigned int mHeight = 0;
			pugi::xml_node root = doc.child("chapter");
			for (auto node : root.children()) {
				if (node.name() == std::string("text")) {
					/* New text object */
					cWikiTextBox* tb = new cWikiTextBox(
						node.text().get(),
						R_FONT_LOOKUP((mConfig)["reader"].find("text")->second["font"]),
						std::stoi((mConfig)["reader"].find("text")->second["size"]),
						0, this->sRect.w, mHeight
					);
					mHeight += ((cTextBox*)tb)->wrappedHeight();
					this->contents.push_back(tb);
				}

				else if(node.name() == std::string("image")) { //For future compatability only (LUL)
					/* New image object. */
					cImage* im = new cImage(
						node.text().get(),
						this->sRect.w/4,
						mHeight,
						this->sRect.w/2,
						-1
					);
					mHeight += ((cImage*)im)->getSize().first;
					this->contents.push_back(im);
				}
			}

			/* Generate texture for this */
			mTex = SDL_CreateTexture(
				mRenderer,
				SDL_PIXELFORMAT_UNKNOWN,
				SDL_TEXTUREACCESS_TARGET,
				w,
				mHeight
			);
			SDL_SetRenderTarget(mRenderer, mTex);
			SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
			SDL_RenderClear(mRenderer);
			for (auto obj : this->contents) {
				obj->render(mRenderer);
			}

			SDL_SetRenderTarget(mRenderer, NULL);


			this->sRect.x = 0;
			this->sRect.y = 0; //TODO: Change when doing the previously up to
			this->loaded = 1;
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
