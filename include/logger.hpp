/* Logger.hpp -> A simple logging system
 * =====================================
 * Justin Huang (MistaRhee)
 *
 * Tl;dr Logger is a simle logging system. Just start up the class and let it do it's thing. Logger
 * can be run on a separate thread (and is recommended to be) however, you can set the flag for it
 * to be a static logger.
 *
 * Licensing (Really? You power hungry people -_-)
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

#ifndef LOGGER_HPP
#define LOGGER_HPP

//Uncomment the following line for no threading (why?)
//#define __NOTHREAD__
#ifndef __NOTHREAD__
#include <mutex>
#include <thread>
#endif

#include <cstdio> //Yes, I'm an old boy. Deal with it
#include <ctime>
#include <queue>
#include <string>

namespace __logger{

    class cLogger{
        public:
            cLogger();
            cLogger(std::string); //Logs to the specified file
            ~cLogger();
            void log(std::string);

#ifndef __NOTHREAD__
            std::thread start();
            void run();
            void kill();
            std::mutex lock;
#endif

        private:
#ifndef __NOTHREAD__
            std::queue<std::string> q;
            bool dead;
#endif
            FILE* flog;

    };
}


#endif //LOGGER_HPP
