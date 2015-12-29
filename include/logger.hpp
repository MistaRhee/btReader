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
//#define __LOGGER_NOTHREAD__
#ifndef __LOGGER_NOTHREAD__
#include <condition_variable>
#include <mutex>
#include <thread>

#define QUEUE_MAX 1000 //Specify the largest size the queue can be before the logger blocks and forces output (to prevent this from eating up large chunks of memory)

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

#ifndef __LOGGER_NOTHREAD__
            std::thread start();
            void run();
            void kill();

            std::mutex done; //A blocking mutex to ensure that all things have been logged before deleting the object (usually used if the main function has a deconstructor or something(
            std::condition_variable cv;
#endif

        private:
#ifndef __LOGGER_NOTHREAD__
            std::queue<std::string> q;
            bool dead;
            std::mutex lock;
#endif
            unsigned int count;
            FILE* flog;

    };
}


#endif //LOGGER_HPP
