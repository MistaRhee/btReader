#include "contents.hpp"

namespace beatOff{

    cNovelList::cNovelList(){
        selected = 0;
        /* Setting "defaults" just in case I forget to set them beforehand */
        fontSize = 25;
        textColour.r = 0;
        textColour.g = 0;
        textColour.b = 0;
        textColour.a = 0;
        backColour.r = 255;
        backColour.g = 255;
        backColour.b = 255;
        backColour.a = 255;
        setFont("system/fonts/droidsansfallback.ttf");
        setFontSize(20);
    }
    
    void cNovelList::setRect(SDL_Rect inRect){
        setPos(inRect.x, inRect.y);
        setSize(inRect.h, inRect.w);
    }

    void cNovelList::addNovel(std::string in, int novelHeight){
        /* Variable height makes my OCD go insane */
        cTextBox newText;
        newText.setText(in);
        newText.setTextSize(fontSize);
        newText.setPos(x, h);
        if(newText.canFit(novelHeight)){
            newText.setSize(w, novelHeight);
        }
        else{
            /* Scale font down so it can actually fit inside the requested
             * height */
            bool canSqueeze = 0;
            for(int i = 1; i < fontSize/2; i++){
                newText.setTextSize(fontSize-i);
                if(newText.canFit(novelHeight)){
                    newText.setSize(w, novelHeight);
                    canSqueeze = 1;
                    break;
                }
            }
            if(!canSqueeze){
                /* If I can't squeeze it in by making the font smaller, cut the
                 * title off to make it fit */
                newText.setTextSize(fontSize);
                while(true){
                    in.pop(); //Erase the back character
                }
            }
        }
        newText.showBox();
        newText.setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
        newText.setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);
        newText.setFont(fontLoc);
        newText.centre(); //Because text centering is nice
        mNovels.push_back(newText);
        h += novelHeight;
    }

    void cNovelList::moveSelection(int ds){
        mNovels[selected].setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
        mNovels[selected].setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);
        selected += ds;
        mNovels[selected].setTextCol(255 - textColour.r, 255 - textColour.g, 255 - textColour.b, 255 - textColour.a);
        mNovels[selected].setBoxCol(255 - backColour.r, 255 - backColour.g, 255 - backColour.b, 255 - backColour.a);

        /* Check to see if selection is on screen, if it isn't, move screen
         * such that it is */
        int reqY = 0; //Required value of Y to make the novel be see-able (is this a word?)
        for(int i = 0; i < selected; i++){
            
        }
    }

    std::string cNovelList::getSelected(){
        return mNovels[selected].getText();
    }

    void cNovelList::render(SDL_Renderer* mRenderer){
        for(int i = 0, j = mNovels.size(); i < j; i++){
            mNovels[i].render(mRenderer);
        }
    }

    void cNovelList::handleUserScroll(int dx, int dy){
        move(0, dy);
    }

    void cNovelList::handleUserMouse(int x, int y, int mouseType, bool isPressed){
        /* Time to do funky math to work out what they clicked */

    }

}
