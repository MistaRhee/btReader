//This is a wikiParser specifically for Media Wiki formatting as used by a
//mojority of wiki pages, however there are other forms of wiki formatting sets
//available like the one on github who uses a markdown style where the symbols
//use a different format. This code is written trusting that nobody will change
//the entire wiki site of Baka-Tsuki which uses MediaWiki formatting.
//
//
//This copde is written as is and users use this code at their own discretion.
//You may edit or update code as you wish as long as you reference this
//opriginal code. In terms of processional and commercial use, well this code
//is written by a random person without a given name, so you are free to use at
//your own risk I guess, I cbf sueing anyone over something that I wrote while
//travelling to and from uni.
//
//Code Written by NoOne2246.


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
    while(original[i] == ' '){                              //remove all spaces at beginning of the line.
        i++;
    }
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
            case '<':                                                   //remove html comments
                if (original[i+1] == '!' && original[i+2] == '-' && original[i+3] =='-'){
                    while(original[i] != '-' && original[i+1] != '-' && original[i+2] != '>'){
                        i++;
                    }
                    i += 2;
                }else if(original[i+1] == 'b' && original[i+2] == 'r' && original[i+3] ==' '){      //remove break lines and put in \n
                    while(original[i] != '>'){
                        i++;
                    }
                    cleaned += '\n';
                }else{
                    cleaned += original[i];
                }
                break;
            case '{':                   //remove template;
                if(original[i+1] == '{'){
                	while(original[i] != '}' && original[i+1] != '}'){
                    	i++;
                    }
                    i++;
                }else{
                    cleaned += original[i];
                break;
			default:
				cleaned += original[i];
                break;
		}
    }
    return cleaned;
}

void cWikiParser::open(const std::string inFile, const std::string existFile){
    cWikiParser::close();                           //clears all the global maps and vectors and other random variables
    FILE*fin = fopen(inFile.c_str(), "r");          //opens the text imnformation file that you got from alling the website AP
	FILE*fexist = fopen(existFile.c_str(), "r");    //opens the file containting whether or not a page exist, you will find in Database.cpp, this page is generated from the XML call.
    char buffer[4096];
    

    database.push_back({"h", 1, "title"});           //well the title is ignored in this anyways, so might put this entry in so you can get stuff immediately under the title.

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
        int position = 0;
        switch(buffer[position]){
            case '<':                                               //only deal with breaks
				if (buffer[1] == 'b' && buffer[2] == 'r'){
					while(buffer[position] != '>'){
						position++;
					}
                }
                database[database.size()-1].text += "\n "; //check this line
                break;
			case ' ':
                position++;     //should not be needed, but in case of some stuff up.
				break;
			case '=':       //found some sort of heading, whether main or sub.
                do{
                    position++;
					level++;
                }while(buffer[position]=='=');
                //create new reqistry item
                database.push_back(Wikitext());
                int counter = database.size()-1;
                //define at heading
                database[counter].type = 'h';
                //set level to equal position
                database[counter].level = level;
                //get data
                //clean the string
                buffer.erase(0, position);
                //put text in.
                database[counter].text = cWikiParser::textCleaner(buffer);
    
                break;
            case ':':       //found some sort of indent
                    //count number of indents
                    //determine if there is an * or some other list symbol.
                    //if yes, then it will check if there prevType was body, if
                    //yes, then it will set a blockquote markup;
                    //equal to number of ':' + 1;
                 break;
            case '*':
                 do{
                     position++;
                     level++;
                 }while(buffer[position] == '*');
                 buffer.erase(0, position);
                 database.push_back({'l', level, cWikiParser::textCleaner(buffer)});

                break;
            case '{':                   //remove template;
                if(original[i+1] == '{'){
                	while(original[i] != '}' && original[i+1] != '}'){
                    	i++;
                    }
                    i++;
                }else{
                default:
                    //figure out current type 
                    counter = database.size()-1;
                    //put it into correct data entry.
                    if(databse[counter].type == 'b' && database[counter].level == 1){
                        database[counter].text += cWikiParser::textCleaner(buffer);
                    }else{
                        database.push_back({"b", 1, information});
                    }
                    break;
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

int cWikiParser::externalLink(std::string original){
    std::string link;
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

    while(original[i] != ' '){
        link += original[i];
        i++;
    }
    while(original[i] == ' '){
        i++;
    }
    for(;i<j; i++){
        text += original[i];
    }
    linkDB[UID].link = link;
    linkDB[UID].text = text;
    linkDB[UID].internal = false;
    linkDB[UID].available = true;
    linkDB[UID].page = true;
    return UID;
}

int cWikiParser::internalLink(std::string original){
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
    while(original[i] != '|' || original[i] != ']'){      //get link up to the first grep;
        link += original[i];
    }
    
    while(original[j] != '|' || original[j] != '['){        //get words from end such as the words in the link or the words under an image
        text += original[j] + text;
        j--;
    }

    link = space2Undersc(link);
    if((link[0] == 'i' && link[1] == 'm' && link[2] == 'a' && link[3] == 'g' && link[4] == 'e')||(link[0] == 'f' && link[1] =='i' && link[2] == 'l' && link[3] == 'e'){
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
