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
    assert(dirExists("/tmp/"));
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
    const char** margv = new const char* [3];
    margv[0] = "/usr/bin/xdg-open";
    margv[1] = loc.c_str();
    margv[2] = NULL;
    execv("/usr/bin/xdg-open", (char**) margv);
}
#endif

XMLNode createHTMLHeader(){
    XMLNode rVal = XMLNode::createXMLTopNode(html);
    rVal.addChild("head");
    rVal.addChild("body");
    rVal.getChildNode("head").addChildNode("title");
    rVal.getChildNode("head").addChildNode("meta");
    rVal.getChildNode("head").getChildNode("meta").addAttribute("charset", "UTF-8");
    return rVal;
}

cWebOut::cWebOut(){
    /* Create the temporary file, ready for writing */
    std::string mLoc;
    mLoc = ctf();
    FILE* ft = fopen(mLoc.c_str(), "w+");
    fclose(ft);
    tempLoc = mLoc;
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
        XMLNode main = createHTMLHeader();
        XMLNode body = main.getChildNode("body");
        main.getChildNode("head").getChildNode("title").addText(title.c_str());
        FILE* fin = fopen(sauce.c_str());
        char buffer[1000000];
        int counter = 0;
        std::string temp;
        /* Bitmask to contain what modifiers are currently used
         * Format:
         *  7 6 5 4 3 2 1 0
         * |R|R|R|B|I|H|H|H|
         *
         * Where 
         * H = Heading # bits (i.e. h1/h2 etc..)
         * I = Italics
         * B = Bold
         * R = Reserved for later (mebbe strikethrough or underlined)
         */
        unsigned char bitmask = 0;
        while(true){ //Loop 'till end of file
            fgets(buffer, 1000000, fin);

            /* Reset counters and bitmask */
            bitmask = 0;
            if(buffer[0] == '='){ //Heading modifier
                /* Count the number of '=' there are */
                int num;
                for(num = 0, int j = strlen(buffer); num < j; num++){
                    if(buffer[num] != '='){
                        break;
                    }
                }
                if(num == j){ //Pure equals, just leave it in
                    body.addChild("p");
                    body.getChildNode(counter).addText(buffer);
                    counter++;
                }
                else{
                    for(int k = 0, int j = strlen(buffer); k < j-num; k++){
                        temp += buffer[k];
                    }
                    sprintf(buffer, "h%d", num);
                    body.addChild(buffer);
                    body.getChildNode(counter).addText(temp);
                    temp.clear();
                    counter ++;
                }
            }
            else{
                for(int i = 0, j = strlen(buffer); i < j; i++){
                }
            }

            if(feof(fin)){ //Now I'm post-checking like a good boy!

            }
        }
    }
}

void cWebOut::cleanUp(){
    remove(tempLoc.c_str());
}
