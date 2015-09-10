#include "btReader.hpp"
#include "wikiParser.hpp"

static const int EXIT = -1;
static const int SYNOPSIS = 0;
static const int VOLUMES = 1;

enum splitter_t{
    locate,
	description,
    spacer,
	novTitle,
    leave	
    //
};

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

std::string cWikiInterpreter::space2Undersc(const std::string title){
    std::string cleaned;
    for(int i = 0, j = title.size(); i<j; i++){
        if(title[i] == ' '){
            cleaned += '_';
        }else{
            cleaned += title[i];
        }
    }
    return cleaned;
}


std::string cWikiInterpreter::generateRandomName(int length){ 
    srand(time(NULL));     
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";       
    std::string rVal;      
    for(int i = 0, j = strlen(aCharacters); i < length; i++){      
        rVal += aCharacters[rand()%j];     
    }      
    return rVal;       
}

void cWikiInterpreter::cleanNovel(const std::string inFile, const std::string existFile, const std::string outFile){
    FILE*fout = fopen(outFile.c_str(), "w+");
    char buffer[4096];
    std::string information;
    int count;
    XMLNode mainNode = XMLNode::createXMLTopNode("novel");
    XMLNode infoNode = mainNode.addChild("info");
    
    cWikiParser wParser;

    wParser.openAP(inFile, existFile);                //open the text file as well as the links availability in the wikiparser.
  /*  
    //get the synopsis, incase someone decides to have headings under synopsis:
    information.clear();
    XMLNode synopsis = mainNode.addChild("synopsis");
    count = cWikiParser.nQuery("synopsis");
    for(int i = 1; i<=count; i++;
        information += cWikiParser.query("synopsis", i) + "\n";
    }
    synopsis.addText(information.c_str());
    information.clear();
    
    count = cWikiParser.nQuery("***");
    XMLNode newVolume = mainNode.addChild("volume");
    newVolume.addAttribute("title", volumeTitle.c_str());
    
    XMLNode chapterNode = newVolume.addChild("chapter");
    chapterNode.addAttribute("title", title.c_str());
    title = "data/novels/"+generateRandomName(50);
    while(fileExists(title)){
        title = "data/novels/"+generateRandomName(50);
    }
    chapterNode.addAttribute("location", title.c_str()); //The chapter will be saved here, it doesn't mean that it will actually have content stored there... That will come later.
    
    chapterNode.addAttribute("dl", "no");
    chapterNode.addAttribute("revid", "");
    

    chapterNode.addAttribute("available", "1");
    
    cGetImage newImageGrab;
    std::string savedTo = newImageGrab.getImage(fileName);
    newVolume.addAttribute("image", savedTo.c_str());
    */
    char *t=mainNode.createXMLString(true);
    fprintf(fout, "%s \n", t);
    fclose(fout);
    free(t);
}

void cWikiInterpreter::cleanChapter(const std::string in, const std::string out){
    FILE*fin = fopen(in.c_str(), "r");
    char buffer[1000000];
    std::string text;
    while(true){
        if(!feof(fin)){
            break;
        }
        else{
            fgets(buffer, 1000000, fin);
            if(buffer[0] == '[' and buffer[1] == '['){
                std::string fileName;
                for(int i = 2;; i++){
                    if(buffer[i] == '|'){
                        break;
                    }
                    else{
                        fileName += buffer[i];
                    }
                }
                cGetImage newImage;
                text += "[[";
                text += newImage.getImage(fileName);
                text += "]]";
            }
            else{
                text += buffer;
                text += "\n";
            }
        }
    }
    fclose(fin);
    fprintf(fopen(out.c_str(), "w+"), "%s \n", text.c_str());
}

std::string cWikiInterpreter::getError(){
    return error;
}

void cWikiInterpreter::setError(std::string inError){
    error = inError;
}
