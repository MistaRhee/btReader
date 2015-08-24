#include "wikiParser.hpp"


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
    char prevType;
    
    int counter = 0;

    database.push_back({"h", 1, "title"});

    //get information on whether an internal link exist 
    int numLinks = 0;
	fscanf(fexist, "%i", &numLinks);	        //get the max number of links to pull out
	int availInt;
	std::map<std::string, int> availMap;        //map with the string and availability
    std::string information;                    //variable forwhich the text is stored into.
	for(int i = 0; i<numLinks;i++){				//pull string and info out
        fgets(buffer, 4096, fexist);
	    std::string volStr;					    //temporary storage of string
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
        //ensure we are not on a blank line in the case of a single \n or <br
        //tag.
        if(buffer[0] != '\n' ||(buffer[0] == '<' && buffer[1] == 'b' && buffer[2] == 'r')){

            //look for any of the following symbols':', '='or "*'
            int position = 0;
            for(int i = 0, j = strlen(buffer); i<j; i++){
            switch(buffer[position]){
                case '=':       //found some sort of heading, whether main or sub.
                    do{
                        position++;
                    }while(buffer[position]=='=')
                    //create new reqistry item
                    database.push_back(Wikitext());
                    //define at heading
                    database[counter].type = 'h';
                    //set level to equal position
                    database[counter].level = position;
                    //get data
                    //clean the string
                        switch(buffer[i]){
                            case '[':

                                break;
                            case ''':

                                break;
                            case '{':                   //remove template;
                                while(buffer[i] != '}'){
                                    i++;
                                }
                                information += buffer[i];
                                break;
                            default:
                                information += buffer[i];
                                break;
                        }
                    }

                    //put text in.
                    database[counter].text = information;
    
                    //increment so that next one can add information correctly.
                    counter++;
                    break;
                case ':':       //found some sort of indent
                    //count number of indents
                    //determine if there is an * or some other list symbol.
                    //if yes, then it will check if there prevType was body, if
                    //yes, then it will set a blockquote markup;
                    //equal to number of ':' + 1;
                    break;
                case '*':

                    break;
                default:
                    //body text
                    //get text into variable information and clean;
    
                    //put it into correct data entry.
                    if(databse[counter-1].type == 'b'){
                        database[counter-1].text = database[counter-1] + information;
                    }else{
                        database.push_back({"b", 1, information});
                        counter++;
                    }
                    break;
            }
        }
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
 

