#include "btReader.hpp"

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

inline bool fileExists(const std::string loc) {
    FILE* test = NULL;
    if (!fopen_s(&test, loc.c_str(), "r")) {
        return true;
    }
    return false;
}

#endif

#if __unix__
#include <unistd.h>
#include <dirent.h>

inline bool fileExists (const std::string& name){
    if(FILE *file = fopen(name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
        return false;
    }
}

inline bool dirExists(const std::string& dirName){
    DIR* myDir = NULL;
    myDir = opendir(dirName.c_str());
    if(myDir == NULL) return false;
    else{
        closedir(myDir);
        return true;
    }
}

/* Same as functions above, just for unix */
inline std::string ctf(){
    if(!dirExists("/tmp/")) throw(mException("Error! User's /tmp doesn't exist!"));
    char tmpNam[L_tmpnam]; //Shouldn't need more than 50 random characters right?
    char* pointer = tmpnam(tmpNam);
    std::string mCurrLoc = tmpNam; 
    while(fileExists(mCurrLoc)){
        pointer = tmpnam(tmpNam);
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

cWebOut::cWebOut(){
    /* Create the temporary file, ready for writing */
    std::string mLoc;
    mLoc = ctf();
#ifdef _WIN32
    FILE* ft = NULL;
    fopen_s(&ft, mLoc.c_str(), "w+"); //Touching the file
    fclose(ft);
#endif
#ifdef __unix__
    FILE* ft = fopen(mLoc.c_str(), "w+");
    fclose(ft);
#endif
    this->tempLoc = mLoc;
    this->isReady = 0;

    /* Create log file */
    std::string logFile = "logs/";
    logFile += currentDateTime();
    logFile += " webOut.log";
    this->mLog = new __logger::cLogger(logFile);
    logFile.clear();
}

cWebOut::cWebOut(__logger::cLogger* mLog) : cWebOut(){
    this->mLog = mLog;
}

cWebOut::~cWebOut(){
    cleanUp();
}

void cWebOut::createPage(std::string sauce, std::string title){
    /* Takes the chapter XML information file and generates a HTML webpage for it */
    if(!fileExists(tempLoc)){
        std::string e = currentDateTime() + ": [webOut.cpp] - Error! Temporary file wasn't created!";
        mLog->log("[webOut.cpp] - Error! Temporary file wasn't created!");
        throw(mException(e));
    }
    else if(!fileExists(sauce)){
        std::string e = currentDateTime() + ": [webOut.cpp] - Error! Sauce file sent doesn't exist!";
        mLog->log("[webOut.cpp] - Error! Sauce file requested doesn't exist!");
        throw(mException(e));
    }
    else{
        /* Start parsing through the file and create a HTML out of it
         * NOTE: This HTML will be a joke of a html page, just H1, H2 etc. + p and text. Don't get
         * hopes up! */
        pugi::xml_document main;
        pugi::xml_node temp = main.append_child("html");
        temp.append_child("head");
        temp.append_child("body");
        temp.child("head").append_child("title");
        temp.child("head").append_child("meta");
        pugi::xml_attribute tAtt = temp.child("head").child("meta").append_attribute("charset");
        tAtt.set_value("UTF-8");
        pugi::xml_node body = temp.child("body");
        temp.child("head").child("title").text().set(title.c_str()); //Safe again, because only one text field in this anyway

        /* Read from file into a string, then decrypt it */
        std::string mFile;
        FILE* fin = NULL;

#ifdef __unix__
        fin = fopen(sauce.c_str(), "r");
#endif
#ifdef _WIN32
        fopen_s(&fin, sauce.c_str(), "r");
#endif
        char buffer[1000000];
        while(fgets(buffer, 1000000, fin)){
            mFile += buffer;
        }
        fclose(fin);

        std::string tmpStr;
        std::string tempS;
        bool bold = 0;
        bool italic = 0;
        while(true){ //Loop 'till end of file (or the end of time, either way is fine)
            for(int i = 0; mFile[i] != '\n'; i++){//Get next line
                tmpStr += mFile[i];
            }
            tmpStr += '\n';
            mFile.erase(mFile.begin(), mFile.begin()+tmpStr.size());

            /* Not resetting flags at the moment because of potential multi-line bold/italics */
            if(tmpStr[0] == '='){ //Heading modifier
                /* Count the number of '=' there are */
                unsigned int num = 0;
                for(num = 0; num < tmpStr.size(); num++){
                    if(tmpStr[num] != '='){
                        break;
                    }
                }
                if(num == tmpStr.size()){ //Pure equals, just leave it in
                    pugi::xml_node tempNode = body.append_child("p");
                    pugi::xml_node tText = tempNode.append_child(pugi::node_pcdata);
                    tempS += tmpStr;
                    tText.set_value(tempS.c_str());
                    tempS.clear();
                }
                else{
                    for(unsigned int k = num; k < tmpStr.size()-num; k++){
                        tempS += tmpStr[k];
                    }
                    pugi::xml_node tempNode = body.append_child((std::string("h")+std::to_string(num)).c_str());
                    pugi::xml_node tText = tempNode.append_child(pugi::node_pcdata);
                    tText.set_value(tempS.c_str());
                    tempS.clear();
                }
            }
            else{
                /* Create a paragraph node */
                pugi::xml_node currNode = body.append_child("p");
                for(unsigned int i = 0; i < tmpStr.size(); i++){
                    /* Continue adding text. */
                    if(tmpStr[i] == '\''){
                        /* Check if it's a triple*/
                        if(i < tmpStr.size() && tmpStr[i+1] == '\'' && tmpStr[i+2] == '\''){
                            /* OH BABY IT'S A TRIPLE 
                             * If it's already flagged, then unflag and display as bold 
                             */
                            if(bold){
                                /* Flagged */
                                bold = 0;
                                if(!tempS.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(tempS.c_str());
                                }
                                tempS.clear();
                                currNode = currNode.parent();
                            }
                            else{
                                bold = 1; //Flip the bit
                                if(!tempS.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(tempS.c_str());
                                }
                                tempS.clear();
                                currNode = currNode.append_child("b");
                            }
                            i += 2;
                        }
                        /* Check for double */
                        else if(i < tmpStr.size()-1 && tmpStr[i+1] == '\''){
                            /* DOUBLE! 
                             * Same as above, just with italics instead of bold
                             */
                            if(italic){
                                /* Flagged */
                                italic = 0; //Clear bit
                                if(!tempS.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(tempS.c_str());
                                }
                                tempS.clear();
                                currNode = currNode.parent();
                            }
                            else{
                                italic = 1; //Flip the bit
                                if(!tempS.empty()){
                                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                    tText.set_value(tempS.c_str());
                                }
                                tempS.clear();
                                currNode = currNode.append_child("i");
                            }
                            i++;
                        }
                        else if(tmpStr[i] == '[' && i < tmpStr.size()-1 && tmpStr[i+1] == '['){ //Should never have a '[[' right next to the end anyway
                            /* Image -> It's an internal link */
                            if(!tempS.empty()){
                                pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                                tText.set_value(tempS.c_str());
                            }
                            tempS.clear();
                            for(unsigned int k = i+2; k < tmpStr.size()-1; k++){
                                if(tmpStr[k] == ']'){
                                    /* Save contents of image, break out */
                                    pugi::xml_node imageNode = currNode.append_child("img");
                                    pugi::xml_attribute imageSauce = imageNode.append_attribute("src");
                                    imageSauce.set_value(tempS.c_str());
                                    i = k+1;
                                    break;
                                }
                                else{
                                    tempS += tmpStr[k];
                                }
                            }
                        }
                        else{
                            /* Just add it to the current text, it's not like it's special or
                             * anything -.- */
                            tempS += tmpStr[i];
                        }
                    }
                    else{
                        tempS += tmpStr[i];
                    }
                }
                if(!tempS.empty()){
                    pugi::xml_node tText = currNode.append_child(pugi::node_pcdata);
                    tText.set_value(tempS.c_str());
                    tempS.clear();
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

void cWebOut::displayPage(){
    disp(tempLoc);
}
