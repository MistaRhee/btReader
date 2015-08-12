#include "mException.h"

mException::mException(std::string in){
    this->message = in;
}

std::string currentDateTime(){
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

const char* mException::what() const throw(){
    return message.c_str();
}
