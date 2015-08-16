#include "btReader.hpp"

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

std::string cWikiParser::space2Undersc(const std::string title){
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


std::string cWikiParser::generateRandomName(int length){ 
    srand(time(NULL));     
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";       
    std::string rVal;      
    for(int i = 0, j = strlen(aCharacters); i < length; i++){      
        rVal += aCharacters[rand()%j];     
    }      
    return rVal;       
}

void cWikiParser::cleanNovel(const std::string inFile, const std::string existFile, const std::string outFile){
    FILE*fin = fopen(inFile.c_str(), "r");
	FILE*fexist = fopen(existFile.c_str(), "r");
    FILE*fout = fopen(outFile.c_str(), "w+");
    char buffer[4096];
    XMLNode mainNode = XMLNode::createXMLTopNode("novel");
    std::string tempStr;

    std::string currentNode; //so we can create chilnode of the correct location.
    std::string currentHeading;//so that we can label all the sub files correctky
    int elementNumber = 0;//attribute with the order of each element;

    int status = SYNOPSIS;
    bool found = 0;
    bool processed = 1;
	bool getting = 1;

    //get information on whether an internal link exist 
    int numLinks = 0;
	fscanf(fexist, "%i", &numLinks);	//get the max number of links to pull out
	int availInt;
	std::map<std::string, int> availMap;//map with the string and availability
	
	for(int i = 0; i<numLinks;i++){				//pull string and info out
        fgets(buffer, 4096, fexist);
	    std::string volStr;					//temporary storage of string
        getting = true;
        for(int i = 0, j = strlen(buffer); i < j; i++){
            if(getting==true){
                if(buffer[i]!='|'){
                    volStr += buffer[i];
                }else{
                    getting = false;
                }
            }else{
                availInt = atoi(&buffer[i]);
                break;
            }
        }
        volStr = space2Undersc(volStr);
		availMap[volStr] = availInt;			//move data into map
        volStr.clear();
	}
    
    while(true){
        if(feof(fin)){
            break;
        }
        if(processed){
            fgets(buffer, 4096, fin);
        }
        if(!processed){
            processed = 1;
        }
        //begin going through text, it will remove all ' ' at beginning of line
        ijnt i = 0;
        while(buffer[i]==' '){
            i++;
        }
        buffer.erase(0, i);

        //look for any of the following symbols':', '='or "*'
        i = 0;
        switch(buffer[i]){
            case '=':
                do{
                    i++;
                }while(buffer[i]=='=')
                if(i ==1){
                    XMLNode header = mainNode.addchild("h1");
                    currentNode = "header";
                    //get the information from heading;
                    headerNode.addAttribute("title", tempStr.c-str());
                    currentHeading = temp;
                }else{
                    switch(i){
                        case 2:
                            XMLNode header2 = currentNode.addchild("h2");
                break;
            case '*':
                break;


                break;
<Down>  default:
                //body text.
                break;
        }


        //if there is no existing headings (variable current heading is clear,
        //so no heading has been detected so far) it will give it a have
        //heading attribute set as title

        //first level is h1 or ==, it will set a title="synopsis" then it will
        //change the variable heading to synopsis
        
        //next level is h2 <so <h2 heading="whatever h1 above it wass"
        //title="text"
    char *t=mainNode.createXMLString(true);
    fprintf(fout, "%s \n", t);
    fclose(fout);
    fclose(fexist);
    free(t);
}

void cWikiParser::cleanChapter(const std::string in, const std::string out){
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

std::string cWikiParser::getError(){
    return error;
}

void cWikiParser::setError(std::string inError){
    error = inError;
}
