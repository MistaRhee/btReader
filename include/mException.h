#ifndef MEXCEPT_H
#define MEXCEPT_H

#include <string>
#include <exception>

struct mException : public std::exception{
    const char* what() const throw(){
        return message.c_str();
    }
    mException(std::string in){
        this->message = in;
    }
    std::string message;
};


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
static std::string currentDateTime(){
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

#endif //MEXCEPT_H
