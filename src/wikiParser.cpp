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

std::string cWikiParser::textCleaner(std::string in){
    std::string original = in; //To remove potential memory space conflicts
    std::string cleaned;
    std::string temp;
    int i = 0;
    int j = original.size();
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
                    cleaned += '{' + internalLink(temp) + '}';         //writes UID in
                    temp.clear();                                                   //clears temp

                }else if(original[i+1] == 'h' && original[i+2]=='t' && original[i+3] == 't' && original[i+4] == 'p'){
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
                }
                break;
			default:
				cleaned += original[i];
                break;
		}
    }
    printf("%s \n", cleaned.c_str());
    return cleaned;
}

/*void open(const std::string link){
    try{
        std::string tempFile;
        std::string novelStore;
        std::string revID;
        std::string progress;
        int exist;
        printf("%s: [database.cpp] - Getting Novel Details for %s. \n", currentDateTime().c_str(), title.c_str());
        cHttpd mDownload;
        tempFile = "data/temp/"+generateRandomName(50);
        while(fileExists(tempFile)){
            tempFile = "data/temp/"+generateRandomName(50);
        }
        mDownload.download(link, tempFile);
        printf("%s: [database.cpp] - Page saved to %s. \n", currentDateTime().c_str(), tempFile.c_str());
        printf("%s: [database.cpp] - Extracting wiki text... \n", currentDateTime().c_str());
        XMLNode mainNode = XMLNode::openFileHelper(tempFile.c_str(), "api");
        XMLNode parseNode = mainNode.getChildNode("parse");
		XMLNode linksNode = parseNode.getChildNode("links");
        revID = parseNode.getAttribute("revid");
        fprintf(fout, "%s", parseNode.getChildNode("wikitext").getText());
        fclose(fout);
		int links = linksNode.nChildNode("pl");	//get the number of links and send it through.	
		for(int i = 0; i<links; i++){
			if(linksNode.getChildNode("pl", i).isAttributeSet("exists")){
                availMap[space2Undersc(linksNode.getChildNode("pl",i).getText())] = 1;
			}
			else{
                availMap[space2Undersc(linksNode.getChildNode("pl",i).getText())] = 1;
			}
		}
        printf("%s: [database.cpp] - Extraction complete! \n", currentDateTime().c_str());
        novelStore = "data/novels/"+generateRandomName(50);
        while(fileExists(novelStore)){
            novelStore = "data/novels/"+generateRandomName(50);
        }
        printf("%s: [database.cpp] - Cleaning novel! Sorry, can't print the name of the file to be saved to due to copyright issues\n", currentDateTime().c_str());
        printf("%s: [database.cpp] - Cleaned page stored in %s. \n", currentDateTime().c_str(), novelStore.c_str());
        printf("%s: [database.cpp] - Deleting temp file \n", currentDateTime().c_str());
        remove (tempFile.c_str());
        return std::make_pair(novelStore, revID);
    }
    catch(mException& e){
        std::string mError = currentDateTime();
        mError += ": [database.cpp] - ";
        mError += e.what();
        setError(e.what());
        printf("%s\n", mError.c_str());
        return std::make_pair("", "");
    }
}*/
void cWikiParser::openAP(const std::string inFile, const std::string existFile){
    close();                           //clears all the global maps and vectors and other random variables
    FILE*fin = fopen(inFile.c_str(), "r");          //opens the text imnformation file that you got from alling the website AP
	FILE*fexist = fopen(existFile.c_str(), "r");    //opens the file containting whether or not a page exist, you will find in Database.cpp, this page is generated from the XML call.
    char data[4096];
    std::string buffer;

    database.push_back({'h', 1, "title"});           //well the title is ignored in this anyways, so might put this entry in so you can get stuff immediately under the title.

    //get information on whether an internal link exist 
    int numLinks = 0;
	fscanf(fexist, "%i", &numLinks);	        //get the max number of links to pull out
    printf("%i\n", numLinks);
	int availInt;
    std::string information;                    //variable forwhich the text is stored into.
    printf("inputed exist map\n");
	for(int i = 0; i < numLinks; i++){				//pull string and info out
        fgets(data, 4096, fexist);
	    std::string volStr;					    //temporary storage of string
        bool getting = true;
        for(int i = 0, j = strlen(data); i < j; i++){  //split the string into title and availability
            if(getting==true){
                if(data[i]!='|'){
                    volStr += data[i];
                }else{
                    getting = false;
                }
            }else{
                availInt = atoi(&data[i]);
                break;
            }
        }
        volStr = space2Undersc(volStr);         //create string using underscores
		availMap[volStr] = availInt;			//move data into map
        printf("%i.%s = %i\n", i, volStr.c_str(), availInt);
        volStr.clear();
	}
    printf("loaded image map\n"); 
    //parse text based on on information.
    bool processed = true;
    while(true){
        if(feof(fin)){                          //loop until end of file
            break;
        }
        if(processed){                          //read data in
            fgets(data, 4096, fin);
            buffer = data;
        }
        printf("%s\n", buffer.c_str());
        if(!processed){
            processed = 1;
        }
        //begin going through text, it will remove all ' ' at beginning of line
        while(buffer[0]==' '){
            buffer.erase(0);
        }
		

            //look for any of the following symbols':', '='or "*'
        int position = 0;
        while(position < buffer.size()){
            printf("%i, %i", position, buffer.size());
            printf("%c\n", buffer[16]);
            getchar();

	    	int level = 0; //counts level the heading or body or information is under.
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
                    database.push_back(WikiText());
                    //define at heading
                    database[database.size()-1].type = 'h';
                    //set level to equal position
                    database[database.size()-1].level = level;
                    //get data
                    //clean the string
                    buffer.erase(0, position);
                    //put text in.
                    database[database.size()-1].text = textCleaner(buffer);
                    break;
                case ':':       //found some sort of indent
                    //count number of indents
                    do{
                        level++;
                        position++; 
                    }while(buffer[position] == ':');
                    //determine if there is an * or some other list symbol.
                    //if yes, then it will check if there prevType was body, if
                    if(buffer[position] != '*'){
                        buffer.erase(0, position);
                        database.push_back({'b', level, textCleaner(buffer)});
                    }else{
                        buffer.erase(0, position);
                        database.push_back({'l', level+1, textCleaner(buffer)});
                    }
                    break;
                case '*':
                     do{
                         position++;
                         level++;
                     }while(buffer[position] == '*');
                     buffer.erase(0, position-1);
                     database.push_back({'l', level, textCleaner(buffer)});
                    break;
                case '{':                   //remove template;
                    if(buffer[position+1] == '{'){
                    	printf("entered correctly\n");
                        while(buffer[position] != '}' && buffer[position+1] != '}'){
                            printf("error here");
                        	position++;
                            printf("%c%c\n", buffer[position], buffer[position+1]);
                        }
                        position = position + 2;
                        printf("exited, next char is %c\n", buffer[position]);
                    }else{
                        buffer.erase(0, position);
                        database.push_back({'b', 1, textCleaner(buffer)});
                    }
                    break;
                case '#':
                     do{
                         position++;
                         level++;
                     }while(buffer[position] == '#');
                     buffer.erase(0, position-1);
                     database.push_back({'n', level, textCleaner(buffer)});
                     break;
                default:
                    //put it into correct data entry.
                    printf("I am here \n");
                    information = textCleaner(buffer);
                    if(database[database.size()-1].type == 'b' && database[database.size()-1].level == 1){
                        database[database.size()-1].text += information;
                    }else{
                        database.push_back({'b', 1, information});
                    }
                    position = buffer.size();
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
    if((link[0] == 'i' && link[1] == 'm' && link[2] == 'a' && link[3] == 'g' && link[4] == 'e')||(link[0] == 'f' && link[1] =='i' && link[2] == 'l' && link[3] == 'e')){
        linkDB[UID].page = false; 
        linkDB[UID].available = true;
    }else{
        linkDB[UID].page = true;
        auto it = availMap.find(link);
        if(it != availMap.end()){
            if(it->second==1){
                linkDB[UID].available = true;
            }else{
                linkDB[UID].available = false;
        	}
        }else{
            printf("[wikiParser.cpp] - Unable to locate %s within map. Set as not avilable for now\n", text.c_str());		//just to make sure error is fixed								
            linkDB[UID].available = false;
        }
    }
    linkDB[UID].link = link;
    linkDB[UID].text = text;
    return UID;
}
