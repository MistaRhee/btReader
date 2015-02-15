#include "httpd.hpp"

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream){
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

cHttpd::cHttpd(){
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
}

cHttpd::~cHttpd(){
    curl_easy_cleanup(curl);
}

std::string cHttpd::fixURL(const std::string incoming){
    std::string rVal;
    for(int i = 0; i < incoming.size(); i++){
        if(incoming[i] == ' '){
            rVal += "%20";
        }
        else{
            rVal += incoming[i];
        }
    }
    return rVal;
}

void cHttpd::download(const std::string url, const std::string fileName){
    FILE* fin;
    std::string newURL = fixURL(url);
    fin = fopen(fileName.c_str(), "w+");
    curl_easy_setopt(curl, CURLOPT_URL, newURL.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    //curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fin);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    fclose(fin);
}

std::string cHttpd::qDownload(const std::string url, int nameLen){
    std::string fileName = generateRandomName(nameLen);
    FILE* fin;
    fin = fopen(fileName.c_str(), "w+");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    //curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fin);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    fclose(fin);
    return fileName;
}

std::string cHttpd::generateRandomName(int length){
    srand(time(NULL));
    const char aCharacters[] = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string rVal;
    for(int i = 0, j = strlen(aCharacters); i < length; i++){
        rVal += aCharacters[rand()%j];
    }
    return rVal;
}
