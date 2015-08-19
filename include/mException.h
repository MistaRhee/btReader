#ifndef MEXCEPT_H
#define MEXCEPT_H

#include <string>
#include <exception>

struct mException : public std::exception{
    const char* what() const throw();
    mException(std::string in);
    std::string message;
};


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
extern std::string currentDateTime();

#endif //MEXCEPT_H
