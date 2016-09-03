#include "hash.hpp"

std::string cCrypt::crypth(std::string string) {
    char toHex[17] = "0123456789abcdef";
    unsigned char h[SHA256_DIGEST_LENGTH];
    std::string rVal;

    SHA256_CTX s;
    SHA256_Init(&s);
    SHA256_Update(&s, string.c_str(), string.length());
    SHA256_Final(h, &s);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        rVal += toHex[(h[i] >> 4) & 0xF];
        rVal += toHex[h[i] & 0xF];
    }

    return rVal;
}