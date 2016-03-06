#ifndef __MCRYPTO__
#define __MCRYPTO__

class cCrypt{
    public:
        char* crypth(const char* string); //Hashes a string
        char* crypts(const char* string, const char* key); //Encrypts a string
        char* decrypts(const char* string, const char* key); //Decrypts a string
};

#endif
