#ifndef __MCRYPTO__
#define __MCRYPTO__

#include <openssl/sha.h>

class cCrypt{
    public:
        char* crypts(const char* string); 
};

#endif
