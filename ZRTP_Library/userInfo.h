#ifndef USERINFO_H
#define USERINFO_H

#include <vector>
#define MAX_ALGORITHM_COUNT 7

typedef std::vector< const char *> info;

// Struct user info represent supported algorithm in this protocol
// it also includes supported protocol version.
struct userInfo{
    info protocolVersion;
    info supportedHashAlgorithm;
    info supportedCipherAlhorithm;
    info supportedAuthTagType;
    info supportedKeyAgreementType;
    info supportedSasType;
};

#endif // USERINFO_H
