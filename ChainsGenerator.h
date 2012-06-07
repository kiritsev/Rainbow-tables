/*
 * ChainsGenerator.h
 *
 *  Created on: May 23, 2012
 *      Author: petr
 */

#ifndef ChainsGenerator_H_
#define ChainsGenerator_H_

#define PASSWORD_LENGHT 8
#define HASH_SIZE 5
#define HASH_BLOCK_CHARACTERS_SIZE 8
#define KEY_FOR_RAND 777
#define CHAIN_LENGTH 1000

class Chain {
public:
    Chain() {
    }

    char head[PASSWORD_LENGHT + 1];
    char tail[PASSWORD_LENGHT + 1];
};

class ChainsGenerator {
public:

    ChainsGenerator();

    virtual ~ChainsGenerator();

    unsigned static callReductionFunction(
            int orderNumber, unsigned* sha1Hash);

    void static callHashFunction(
            char* dataString, unsigned* hash);

    void static callHashFunction(
            unsigned dataNumber, unsigned* hash);

    void static generateChain(
            Chain* resultChain);

    void static generateChainFromPosition(
            int startFunctionNumber, unsigned* sha1Hash,
            Chain* resultChain);

    unsigned static generateChainUntilAimHash(
            unsigned startPassword,    unsigned* sha1Hash);

    unsigned generateRandomPassword();

    void static printSha1(
            unsigned *message_digest);

    void static printSha1Hex(
            unsigned *message_digest);

    bool static compareHashes(
            unsigned* hash1, unsigned* hash2);
};

#endif /* ChainsGenerator_H_ */
