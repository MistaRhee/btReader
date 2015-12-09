#include "btReader.hpp"

inline bool fileExists(const std::string loc){
    FILE* ftest = NULL;
    ftest = fopen(loc.c_str(), "r");
    if(ftest) return 1;
    else return 0;
}

#ifdef _WIN32
#include <windows.h>

inline std::string ctf(){ //Creates temporary file
    char mPath[260];
    char mBuff[260];
    int gpVal = GetTempPath(260, mPath);
    if(gpVal > 260 || gpVal == 0){
        fprintf(stderr, "Windows Error -> There is no temp path? (or path too long)");
        exit(322);
    }
    if(!GetTempFileName(mPath, NULL, 0, mBuff)){
        fprintf(stderr, "Windows Error -> Can't create temp-file name?");
        exit(323);
    }
    std::string rVal = mBuff;
    return rVal;
}

inline void disp(std::string loc){ //Displays webpage in user's default browser
    ShellExecute(NULL, "open", loc.c_str(),
            NULL, NULL, SW_SHOWNORMAL);
}

#endif

#if __unix__
#include <unistd.h>

inline bool dirExists(const std::string& dirName){
    DIR* myDir = NULL;
    myDir = opendir(dirName.c_str());
    if(myDir == NULL) return false;
    else return true;
}

/* Same as functions above, just for unix */
inline std::string ctf(){
    if(!dirExists("/tmp/")) throw(mException("Error! User's /tmp doesn't exist!"));
    char tmpNam[L_tmpnam]; //Shouldn't need more than 50 random characters right?
    tmpnam(tmpNam);
    std::string mCurrLoc = tmpNam; 
    while(fileExists(mCurrLoc)){
        tmpnam(tmpNam);
        mCurrLoc = tmpNam;
    }
    return mCurrLoc;
}

inline void disp(std::string loc){
    pid_t i = fork();
    if(i == 0){
        /* Child process */
        const char** margv = new const char* [3];
        margv[0] = "/usr/bin/xdg-open";
        margv[1] = loc.c_str();
        margv[2] = NULL;
        execv("/usr/bin/xdg-open", (char**) margv);
    }
}
#endif

pugi::xml_document createHTMLHeader(){
    pugi::xml_document rVal;
    pugi::xml_node temp = rVal.append_child("html");
    temp.append_child("head");
    temp.append_child("body");
    temp.child("head").append_child("title");
    temp.child("head").append_child("meta");
    pugi::xml_attribute tAtt = temp.child("head").child("meta").append_attribute("charset");
    tAtt.set_value("UTF-8");
    return rVal;
}

cWebOut::cWebOut(){
    /* Create the temporary file, ready for writing */
    std::string mLoc;
    mLoc = ctf();
    FILE* ft = fopen(mLoc.c_str(), "w+");
    fclose(ft);
    this->tempLoc = mLoc;
    this->isReady = 0;
}

cWebOut::~cWebOut(){
    cleanUp();
}

void cWebOut::createPage(std::string sauce, std::string title){
    /* Takes the chapter XML information file and generates a HTML webpage for it */
    if(!fileExists(tempLoc)){
        std::string e = currentDateTime() + ": [webOut.cpp] - Error! Temporary file wasn't created!";
        throw(mException(e));
    }
    else if(!fileExists(sauce)){
        std::string e = currentDateTime() + ": [webOut.cpp] - Error! Sauce file sent does not exist!";
        throw(mException(e));
    }
    else{
        /* Start parsing through the file and create a HTML out of it
         * NOTE: This HTML will be a joke of a html page, just H1, H2 etc. + p and text. Don't get
         * hopes up! */
        pugi::xml_document main = createHTMLHeader();
        pugi::xml_node body = main.child("body");
        main.child("head").child("title").text().set(title.c_str()); //Safe again, because only one text field in this anyway
        FILE* fin = fopen(sauce.c_str(), "r");
        char buffer[1000000];
        std::string temp;
        bool bold = 0;
        bool italic = 0;
        while(true){ //Loop 'till end of file (or the end of time, either way is fine)
            fgets(buffer, 1000000, fin); //Get next line

            /* Not resetting flags at the moment because of potential multi-line bold/italics */
            if(buffer[0] == '='){ //Heading modifier
                /* Count the number of '=' there are */
                int num = 0;
                int j = strlen(buffer);
                for(num = 0; num < j; num++){
                    if(buffer[num] != '='){
                        break;
                    }
                }
                if(num == j){ //Pure equals, just leave it in
                    pugi::xml_node tempNode = body.append_child("p");
                    pugi::xml_node tText = tempNode.append_child(pugi::node_pcdata);
                    tText.set_value(buffer);
                }
                else{
                    for(int k = 0; k < j-num; k++){
                        temp += buffer[k];
                    }
                    sprintf(buffer, "h%d", num);
                    pugi::xml_node tempNode = body.append_child(buffer);
                    pugi::xml_node tText = tempNode.append_child(pugi::node_pcdata);
                    tText.set_value(temp.c_str());
                    temp.clear();
                }
            }
            else{
                /* Create a paragraph node */
                pugi::xml_node currNode = body.append_child("p");
                for(int i = 0, j = strlen(buffer); i < j; i++){
                    /* Continue adding text. */
                    if(buffer[i] == '\''){
                        /* Check if it's a triple*/
                        if(i < j-2 && buffer[i+1] == '\'' && buffer[i+2] == '\''){
                            /* OH BABY IT'S A TRIPLE 
                             * If it's already flagged, then unflag and display as bold 
                             */
                            if(bold){
                                /* Flagged */
                                bold = 0;
                                if(!temp.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(temp.c_str());
                                }
                                temp.clear();
                                currNode = currNode.parent();
                            }
                            else{
                                bold = 1; //Flip the bit
                                if(!temp.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(temp.c_str());
                                }
                                temp.clear();
                                currNode = currNode.append_child("b");
                            }
                            i += 2;
                        }
                        /* Check for double */
                        else if(i < j-1 && buffer[i+1] == '\''){
                            /* DOUBLE! 
                             * Same as above, just with italics instead of bold
                             */
                            if(italic){
                                /* Flagged */
                                italic = 0; //Clear bit
                                if(!temp.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(temp.c_str());
                                }
                                temp.clear();
                                currNode = currNode.parent();
                            }
                            else{
                                italic = 1; //Flip the bit
                                if(!temp.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(temp.c_str());
                                }
                                temp.clear();
                                currNode = currNode.append_child("i");
                            }
                            i++;
                        }
                        else if(buffer[i] == '[' && i < j-1 && buffer[i+1] == '['){ //Should never have a '[[' right next to the end anyway
                            /* Image -> It's an internal link */
                            if(!temp.empty()){
                                pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                tText.set_value(temp.c_str());
                            }
                            temp.clear();
                            for(int k = i+2; k < j-1; k++){
                                if(buffer[k] == ']'){
                                    /* Save contents of image, break out */
                                    pugi::xml_node imageNode = currNode.append_child("img");
                                    pugi::xml_attribute imageSauce = imageNode.append_attribute("src");
                                    imageSauce.set_value(temp.c_str());
                                    i = k+1;
                                    break;
                                }
                                else{
                                    temp += buffer[k];
                                }
                            }
                        }
                        else{
                            /* Just add it to the current text, it's not like it's special or
                             * anything -.- */
                            temp += buffer[i];
                        }
                    }
                    else{
                        temp += buffer[i];
                    }
                }
                if(!temp.empty()){
                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                    tText.set_value(temp.c_str());
                    temp.clear();
                }
            }

            if(feof(fin)){ //Now I'm post-checking like a good boy!
                break;
            }
        }
    }
}

void cWebOut::cleanUp(){
    remove(tempLoc.c_str());
}
