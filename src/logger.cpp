/* Logger.hpp -> A simple logging system
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
 *  Copyright (C) 2015 Justin Huang
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
        this->flog = fopen("wooden.log", "w+");
        if(!this->flog){
            fprintf(stderr, "Critical Hit! flog failed to open the file! \n");
            exit(322);
        }

#ifndef __NOTHREAD__
        this->dead = 0;
#endif
    }

    cLogger::cLogger(std::string fileLoc){
        flog = NULL;
        flog = fopen(fileLoc.c_str(), "w+");
        if(!flog){
            fprintf(stderr, "Critical Hit! flog failed to open the file! \n");
            exit(322);
        }
#ifndef __NOTHREAD__
        this->dead = 0;
#endif
    }
        
    cLogger::~cLogger(){
#ifndef __NOTHREAD__
        this->dead = 1;
#endif
        fflush(flog);
        fclose(flog);
    }

    void cLogger::log(std::string out){
        std::string output = currentDateTime() + ": ";
        output += out;
        while(output[output.size()-1] == '\n') output.erase(output.end()-1); //Remove trailing new-lines
#ifdef __NOTHREAD__
        fprintf(this->flog, "%s \n", output.c_str());
#else
        this->lock.lock();
        q.push(output);
        this->lock.unlock();
#endif
    }

#ifndef __NOTHREAD__
    std::thread cLogger::start(){
        return std::thread(&cLogger::run, this);
    }

    void cLogger::run(){
        while(!this->dead){
            std::this_thread::sleep_for(std::chrono::seconds(1)); //So I don't rape the CPU
            this->lock.lock();
            if(!this->q.empty()){ //Doing slow output so input can be fast
                std::string out = this->q.front();
                this->q.pop();
                fprintf(this->flog, "%s \n", out.c_str());
                fflush(this->flog); //Force the update
            }
            this->lock.unlock();
        }
        while(!this->q.empty()){
            std::string out = this->q.front();
            this->q.pop();
            fprintf(this->flog, "%s \n", out.c_str());
        }
    }

    void cLogger::kill(){
        this->dead = 1;
    }

#endif
}
