/* Logger.cpp -> A simple logging system
 * =====================================
 * Justin Huang (MistaRhee)
 *
 * Tl;dr Logger is a simle logging system. Just start up the class and let it do it's thing. Logger
 * can be run on a separate thread (and is recommended to be) however, you can set the flag for it
 * to be a static logger.
 *
 * Licensing (Re-iterating what logger.hpp said)
 * =========
 * 
 *  Logger: a simple logging system
 *  Copyright (C) 2015 - 2016 Justin Huang
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program.  If not, see
 *  <http://www.gnu.org/licenses/>
 * ***************************************************************************************************/

#include "logger.hpp"

#ifdef _WIN32
#include <windows.h>

inline bool dirExists(const std::string& dirName) {
    DWORD ftyp = GetFileAttributesA(dirName.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif

#if __unix__
#include <dirent.h>

inline bool dirExists(const std::string& dirName){
    DIR* myDir = NULL;
    myDir = opendir(dirName.c_str());
    if(myDir == NULL) return false;
    else return true;
}

inline void createFolder(const std::string& dirName){
    std::string command;
    command = "mkdir "+dirName;
    system(command.c_str());
}
#endif

inline bool fileExists (const std::string& name){
    if(FILE *file = fopen(name.c_str(), "r")){
        fclose(file);
        return true;
    }
    else{
        return false;
    }
}

namespace __logger{

    inline std::string currentDateTime(){
        time_t now = time(0);
        struct tm tstruct;
        char buff[100];
        tstruct = *localtime(&now);
        strftime(buff, sizeof(buff), "%Y-%m-%d.%X", &tstruct);

        return std::string(buff);
    }

    cLogger::cLogger(){
        this->flog = NULL;
        this->count = 0;
        this->flog = fopen("wooden.log", "w+");
        if(!this->flog){
            fprintf(stderr, "Critical Hit! flog failed to open the file! \n");
            exit(322);
        }

#ifndef __LOGGER_NOTHREAD__
        this->dead = 0;
#endif
    }

    cLogger::cLogger(std::string fileLoc){
        /* Check if there is a folder in the location -> ensure that the folder exists */
        std::string tempStr;
        this->count = 0;
        if(fileExists(fileLoc)){
            /* Rename file to be backup */
            if(rename(fileLoc.c_str(), (fileLoc + std::string(".old")).c_str())) fprintf(stderr, "Renaming failed! Overwriting old file \n");
            else fprintf(stderr, "Previous log was successfully renamed to <prev>.old \n");
        }
        else{
            for(unsigned int i = 0; i < fileLoc.size(); i++){
                if(fileLoc[i] == '/'){
                    /* There is folder */
                    if(!dirExists(tempStr)){
                        if(!tempStr.empty()) createFolder(tempStr);
                    }
                }
                else{
                    tempStr += fileLoc[i];
                }
            }
        }
        tempStr.clear();
        this->flog = NULL;
        this->flog = fopen(fileLoc.c_str(), "w+");
        if(!this->flog){
            fprintf(stderr, "Critical Hit! flog failed to open the file! \n");
            exit(322);
        }
#ifndef __LOGGER_NOTHREAD__
        this->dead = 0;
#endif
    }

    cLogger::~cLogger(){
        fflush(flog);
        fclose(flog);
    }

    void cLogger::log(std::string out){
        std::string output = currentDateTime() + std::string(" ") + out;
        while(output[output.size()-1] == '\n') output.erase(output.end()-1); //Remove trailing new-lines
#ifdef __LOGGER_NOTHREAD__
        fprintf(this->flog, "%s \n", output.c_str());
#else
        std::unique_lock<std::mutex> ul(this->lock);
        cv.notify_one();
        q.push(output);
#endif
    }

#ifndef __LOGGER_NOTHREAD__
    std::thread cLogger::start(){
        return std::thread(&cLogger::run, this);
    }

    void cLogger::run(){
        this->done.lock();
        while(!this->dead){
            std::unique_lock<std::mutex> ul(this->lock);
            while(this->q.empty()) cv.wait(ul);
            if(!this->q.empty()){ //Doing slow output so input can be fast
                do{
                    std::string out = this->q.front();
                    this->q.pop();
                    fprintf(this->flog, "%s \n", out.c_str());
                    fflush(this->flog); //Force the update
                }
                while (this->q.size() > QUEUE_MAX);
            }
        }
        while(!this->q.empty()){
            std::string out = this->q.front();
            this->q.pop();
            fprintf(this->flog, "%s \n", out.c_str());
        }
        this->done.unlock();
    }

    void cLogger::kill(){
        this->dead = 1;
    }

#endif
}
