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

#endif

#if __unix__
#include <unistd.h>

inline bool dirExists(const std::string& dirName){
    DIR* myDir = NULL;
    myDir = opendir(dirName.c_str());
    if(myDir == NULL) return false;
    else return true;
}

inline bool fileExists(const std::string loc){
    FILE* ftest = NULL;
    ftest = fopen(loc.c_str(), "r");
    if(ftest) return 1;
    else return 0;
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

cWebOut::cWebOut(){
    /* Create the temporary file, ready for writing */
}

cWebOut::~cWebOut(){
    cleanUp();
}

void cWebOut::createPage(std::string sauce){
    /* Takes the chapter XML information file and generates a HTML webpage for it */
}

void cWebOut::cleanUp(){
    remove(tempLoc.c_str());
}
