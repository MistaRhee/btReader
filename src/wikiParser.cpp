#include "wikiParser.hpp"

typedef struct _links_info{
    bool link;
    bool internal;
    bool available;
    std::string link;
    std::string text;
}links_info;


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


void cWikiParser::open(const std::string inFile, const std::string existFile){
    FILE*fin = fopen(inFile.c_str(), "r");
	FILE*fexist = fopen(existFile.c_str(), "r");
    char buffer[4096];
    int currentHeading;//so that we can label all the sub files correctky
    int elementNumber = 0;//attribute with the order of each element;
    headNames[1] = "title";
    currentHeading = 1;

    //get information on whether an internal link exist 
    int numLinks = 0;
	fscanf(fexist, "%i", &numLinks);	//get the max number of links to pull out
	int availInt;
	std::map<std::string, int> availMap;//map with the string and availability
	
	for(int i = 0; i<numLinks;i++){				//pull string and info out
        fgets(buffer, 4096, fexist);
	    std::string volStr;					//temporary storage of string
        getting = true;
        for(int i = 0, j = strlen(buffer); i < j; i++)  //split the string into title and availability{
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
        volStr = space2Undersc(volStr);         //create string using underscores
		availMap[volStr] = availInt;			//move data into map
        volStr.clear();
	}
   
    //parse text based on on information.
    while(true){
        if(feof(fin)){                          //loop until end of file
            break;
        }
        if(processed){                          //read data in
            fgets(buffer, 4096, fin);
        }
        if(!processed){
            processed = 1;
        }
        //begin going through text, it will remove all ' ' at beginning of line
        while(buffer[0]==' '){
            buffer.erase(0);
        }

        //look for any of the following symbols':', '='or "*'
        int position = 0;
        switch(buffer[position]){
            case '=':       //found some sort of heading, whether main or sub.
                do{
                    position++;
                }while(buffer[position]=='=')
                //get data
                //clean the string
                //create new reqistry item
                //define at heading
                //set level to equal position
                //put text in.
                break;
            case ':':       //found some sort of indent
                break;
            case '*':

                break;
            default:
                //body text
                break;
        }


    fclose(fin);
    fclose(fexist);
}

void cWikiParser::close(){
}


std::string cWikiParser::textMarkUp(const std::string original){
    std::string cleaned;
    //parse text to markup links and images with unique ID
    //markup to rmove {}
    //also put markup for italics and bold
}

struct cWikiParser::linksChecker(std::string original){{
    /* std::string site;
    std::string text;
    bool link;
    bool internal;
    bool available;*/
    while(original[i]!='['){         //get the link checker togo up to the [ at the begging, this should be automatic, but just in case{
        i++;
    }
    while(original[i] =='['){       //goes to the first character of the string/
        i++;
    }
    if(original[i] == 'h' && original[i+1]=='t' && original[i+2] == 't' && original[i+3] == 'p'){
        links_info.link = true;
        links_info.internal = false;
        links_info.available = false;
        while(original[i] != ' '){
            links_info.link += original[i];
            i++;
        }
        i++;
        while(original[i] != ']'){
            links_info.text += original[i];
            i++;
        }
        else{
            int j= original.size();
            while(original[i] == ' '){
                j--;
             }
             while(original[j] == ']'){
                j--;
             }
             while(original[j] != '|' || original[j] != '['){
                links_info.text = original[j] + links_info.text;
                j--;
             }
             if(original == '['){
                links_info.link = false;
                links_info.internal = false;
                links_info.available = false;
            }else{
                links_info.link = true;
                links_info.internal = true;
                j--;
                while(original[j] != '['){
                    links_info.site = original[j] + links_info.site;
                    j--;
                }
                links_info.site = space2Undersc(links_info.site);
                auto it = availMap.find(chapName);
                if(it != availMap.end()){
                    if(it->second==1){
                        links_info.available = true
                    }else{
                        links_info.availbale = false;
            	    }
                }else{
	                printf("%s:[wikiParser.cpp] - Unable to locate %s within map. Set as not avilable for now\n", currentDateTime().c_str(), chapName.c_str());		//just to make sure error is fixed								
                    available = false;//this could indicate images.
                }
            }
        }
    }
}
 

