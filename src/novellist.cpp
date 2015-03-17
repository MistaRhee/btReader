#include "objects.hpp"

namespace beatOff{
    
    cNovelList::cNovelList(SDL_Rect* inRect){
        setPos(inRect->x, inRect->y);
        setSize(inRect->h, inRect->w);
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

    void cNovelList::addNovel(std::string in){
        cTextBox newText;
        newText.setText(in);
        newText.setPos(x, h);
        newText.setSize(w, novelHeight);
        newText.setTextSize(fontSize);
        newText.showBox();
        newText.setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
        newText.setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);
        newText.setFont(fontLoc);
        mNovels.push_back(newText);
        h += novelHeight;
    }

    void cNovelList::moveSelection(int ds){
        mNovels[selected].setTextCol(textColour.r, textColour.g, textColour.b, textColour.a);
        mNovels[selected].setBoxCol(backColour.r, backColour.g, backColour.b, backColour.a);
        selected += ds;
        mNovels[selected].setTextCol(255 - textColour.r, 255 - textColour.g, 255 - textColour.b, 255 - textColour.a);
        mNovels[selected].setBoxCol(255 - backColour.r, 255 - backColour.g, 255 - backColour.b, 255 - backColour.a);
    }

    std::string cNovelList::getSelected(){
        return mNovels[selected].getText();
    }

    void cNovelList::render(SDL_Renderer* mRenderer){
        for(int i = 0, j = mNovels.size(); i < j; i++){
            mNovels[i].render(mRenderer);
        }
    }

}
