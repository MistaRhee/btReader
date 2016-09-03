/* A replacing the experimental crypto library that I was developing/using. Shares same function names as mCrypto for compatibility in future. */

#ifndef HASH_HPP
#define HASH_HPP

#include <openssl/sha.h>
#include <string>

class cCrypt{
    public:
        static std::string crypth(std::string string); //Hashes a string
};

#endif //HASH_H