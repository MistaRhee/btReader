#ifndef HTTPD_HPP
#define HTTPD_HPP

//A class which basically just takes in a URL and a file name for it to save
//that URL to. Nothing that special.

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>
#include <cstdio>

class cHttpd{
    public:
        cHttpd();
        ~cHttpd();
        void dowload(const std::string, const std::string);
    private:
        void* curl;
};
