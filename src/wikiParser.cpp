#include "wikiParser.hpp"

struct {
    bool bold = false;
    bool italic = false;
}charStates;
        

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

std::string cWikiParser::textCleaner(const std::string original){
    std::string cleaned;
    std::string temp;
    int i = 0;
    int counter = 0;
    int j = strlen(original);
    for(;i<j; i++){
        switch(original[i]){
            case '[':                                       //found beginning of bracket, check next few if it is a internal or external link
                if(original[i+1] =='['){                    //check if external link
                    i += 2;
                    while(original[i] != ']' && original[i+1] != ']'){              //read link into temp
                        temp += original[i];
                        i++;
                    }
                    i++;
                    cleaned += '{' + cWikiParser::internalLink(temp) + '}';         //writes UID in
                    temp.clear();                                                   //clears temp

                }else if(original[1] == 'h' && original[i+2]=='t' && original[i+3] == 't' && original[i+4] == 'p'){
                                                                        //http link means external
                    i++;
                   while(original[i] != ']'){                           //get information
                        temp += original[i];
                        i++;
                    }
                    cleaned += '{' + externalLink(temp) + '}';          //put into string
                    temp.clear();
                }else{                                                  //not a string
                    cleaned += original[i];
                }
                break;
/*            case '':	//how do I check for apostrophe symbol????
				level = 0;
				do{
					i++;
					level++;
				}while(buffer[i] == '&apos');
				switch (level){
					case 0:
						information += '&lsquo'; //put in left quotes
						break;
					case 1:
						if (charStates.italic == true){
                            information += '</i>';//italicise the text
                        }else{
                            information += '<i>';//italicise the text
                        }
                        charStates.italic = !charStates.italic;
						break;
					case 2:
						if (charStates.bold == true){
						    information += '</b>';//bold text
                        }else{
						    information += '<b>';//bold text
                        }
                        charStates.bold = !charStates.bold;
						break;
					case 3:                 //change both bold and italics
						if (charStates.italic == true){
                            information += '</i>';//italicise the text
                        }else{
                            information += '<i>';//italicise the text
                        }
                        charStates.italic = !charStates.italic;
						if (charStates.bold == true){
						    information += '</b>';//bold text
                        }else{
						    information += '<b>';//bold text
                        }
                        charStates.bold = !charStates.bold;
						break;
					default:
						printf("wtf did the person writing their page do? put bold or italicised apostrophes???");
						break;
				}
                break;*/
            case '{':                   //remove template;
            	while(buffer[position] != '}'){
                	position++;
                }
                break;
			default:
				cleaned += buffer[position];
                break;
		}
    }
    return cleaned;
}

void cWikiParser::open(const std::string inFile, const std::string existFile){
    cWikiParser::close;
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
		
		int level = 0; //counts level the heading or body or information is under.

            //look for any of the following symbols':', '='or "*'
        for(int position = 0, length = strlen(buffer); position<length; position++){
            switch(buffer[0]){
                case '<':
					if (buffer[1] == 'b' && buffer[2] == 'r'){
						while(buffer[position] != '>'){
							position++;
						}
					}
				case ' ':
					break;
				case '=':       //found some sort of heading, whether main or sub.
                    do{
                        position++;
						level++;
                    }while(buffer[position]=='=')
                    //create new reqistry item
                    database.push_back(Wikitext());
                    //define at heading
                    database[counter].type = 'h';
                    //set level to equal position
                    database[counter].level = level;
                    //get data
                    //clean the string
					for(;position < length; position++);//check this if statement as I do not know whether position or something is needed as first arguement.
                        switch(buffer[position]){
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
    //clear maps and database;
    availMap.clear();
    database.clear();
    linkDB.clear();
}


int cWikiParser::internalLink(std::string original){{
    /* std::string site;
    std::string text;
    bool link;
    bool internal;
    bool available;
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
*/


    //begin code;
            
    //get to the pipe
    //check if image
    //if image store,
    //else get after pipe and ckeck avail
    std::string link;       //store the link information pulled;
    std::string text;
    linkDB.push_back(Links());
    
    int UID = linkDB.size()-1;                  //size will get number of elements so the first one is linkDB[0] while size is 1

    int i = 0;
    while(original[i] ==' '){                   //remove space at front, probs redundant unless someone fucked up.
        i++;
    }

    int j = original.size();                    //figure out space at the end
    while(original[j] == ' '){
        j--;
    }

    //for(;i<j;i++){
    while(original[i] != '|'){      //get link up to the first grep;
        link += original[i];
    }
    
    while(original[j] != |){        //get words from end such as the words in the link or the words under an image
        text += original[j] + text;
        j--;
    }

    link = space2Undersc(link);
    if(link[0] == 'i' && link[1] == 'm' && link[2] == 'a' && link[3] == 'g' && link[4] == 'e'){
        linkDB[UID].page = false; 
        linkDB[UID].available = true;
    }else{
        linkDB[UID].page = true;
        auto it = availMap.find(link);
        if(it != availMap.end()){
            if(it->second==1){
                linkDB[UID].available = true
             }else{
                linkDB[UID].availbale = false;
        	}
        }else{
            printf("%s:[wikiParser.cpp] - Unable to locate %s within map. Set as not avilable for now\n", currentDateTime().c_str(), chapName.c_str());		//just to make sure error is fixed								
            linkDB[UID].available = false;
        }
    }
    linkDB[UID].link = link;
    linkDB[UID].text = text;
    return UID;
}
 

