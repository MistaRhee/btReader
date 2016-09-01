#ifndef MEXCEPT_H
#define MEXCEPT_H

#include <ctime>
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
inline std::string currentDateTime(){
#ifdef __unix__
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
#endif
#ifdef _WIN32
    char buf[32];
    struct tm newTime;
    __time32_t now;
    _time32(&now);

    _localtime32_s(&newTime, &now);
    asctime_s(buf, 32, &newTime);
#endif
    return std::string(buf);
}

#endif //MEXCEPT_H
