#ifndef MEXCEPT_H
#define MEXCEPT_H

#include <exception>

struct mException : public std::exception{
    mException(std::string in){
        this->message = in;
    }
    std::string message;
    const char* what() const throw(){
        return message.c_str();
    }
};

#endif //MEXCEPT_H
