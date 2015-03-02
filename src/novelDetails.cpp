#include "objects.hpp"

namespace beatOff{

    cNovelDetails::cNovelDetails(SDL_Rect* inRect){
        loaded = 0;
        setPos(inRect->x, inRect->y);
        setSize(inRect->h, inRect->w); //Like all things, height is variable, its just the width that stays the same
    }

    void cNovelDetails::getContent(std::string sauce){
        try{
            XMLNode mainNode = XMLNode::openFileHelper(sauce.c_str(), "novel");
            title = mainNode.getChildNode("info").getAttribute("title");
            author = mainNode.getChildNode("info").getAttribute("author");
            synopsis = mainNode.getChildNode("synopsis").getText();
            for(int i = 0, j = mainNode.nChildNode("volume"); i < j; i++){
                XMLNode volume = mainNode.getChildNode("volume", i);
                for(int k = 0, l = volume.nChildNode("chapter"); k < l; k++){
                    
                }
            }
        }
        catch(mException& e){
           setError(e.what()); 
        }
    }

}

