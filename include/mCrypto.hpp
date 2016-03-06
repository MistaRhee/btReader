#ifndef __MCRYPTO__
#define __MCRYPTO__

class cCrypt{
    public:
        static char* crypth(const char* string); //Hashes a string
        static char* crypts(const char* string, const char* key); //Encrypts a string
        static char* decrypts(const char* string, const char* key); //Decrypts a string
};

#endif
