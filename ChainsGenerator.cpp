/*
 * ChainsGenerator.cpp
 *
 *  Created on: May 23, 2012
 *      Author: petr
 */

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "sha1/sha1.h"
#include "ChainsGenerator.h"
#include "cudaFunctions.h"

using namespace std;

ChainsGenerator::ChainsGenerator() {
    srand(KEY_FOR_RAND);
}

ChainsGenerator::~ChainsGenerator() {
}

unsigned ChainsGenerator::generateRandomPassword() {
    int i;
    unsigned password = 0;

    password = rand() % 10;
    for (i = 0; i < PASSWORD_LENGHT - 1; i++) {
        password *= 10;
        password += rand() % 10;
    }

    return password;
}

void ChainsGenerator::generateChain(Chain* resultChain) {
    unsigned hash[HASH_SIZE];
    unsigned password;
    int i;

    password = atoi(resultChain->head);

    for (i = 0; i < CHAIN_LENGTH; i++) {
        callHashFunction(password, hash);
        password = callReductionFunction(i, hash);
    }

    sprintf(resultChain->tail, "%u", password);
}

void ChainsGenerator::generateChainFromPosition(
        int startFunctionNumber,
        unsigned* sha1Hash, Chain* resultChain) {
    int i;
    unsigned hash[HASH_SIZE];
    unsigned password = callReductionFunction(
            startFunctionNumber, sha1Hash);

    for (i = startFunctionNumber + 1; i < CHAIN_LENGTH; ++i) {
        callHashFunction(password, hash);
        password = callReductionFunction(i, hash);
    }

    sprintf(resultChain->tail, "%u", password);
}

unsigned ChainsGenerator::generateChainUntilAimHash(
        unsigned startPassword, unsigned* sha1Hash) {
    unsigned hash[HASH_SIZE];
    unsigned password = startPassword;
    int i;

    for (i = 0; i < CHAIN_LENGTH; i++) {
        callHashFunction(password, hash);

        if (compareHashes(sha1Hash, hash)) {
            return password;
        }

        password = callReductionFunction(i, hash);
    }

    return 0;
}

bool ChainsGenerator::compareHashes(
        unsigned* hash1, unsigned* hash2) {
    int i;
    for (i = 0; i < HASH_SIZE; ++i) {
        if (hash1[i] != hash2[i]) {
            return false;
        }
    }
    return true;
}

void ChainsGenerator::callHashFunction(
        unsigned dataNumber, unsigned* hash) {
    char dataString[PASSWORD_LENGHT + 2];
    sprintf(dataString, "%u\0", dataNumber);
    callHashFunction(dataString, hash);
}

void ChainsGenerator::callHashFunction(
        char* dataString, unsigned* hash) {
    SHA1Context sha;

    SHA1Reset(&sha);
    SHA1Input(&sha, (const unsigned char *) dataString, strlen(dataString));

    if (!SHA1Result(&sha)) {
        fprintf(stderr, "ERROR: could not compute hash\n");
    } else {
        memcpy(hash, &sha.Message_Digest, HASH_SIZE * sizeof(unsigned));
    }
}

unsigned ChainsGenerator::callReductionFunction(int orderNumber,
        unsigned* sha1Hash) {
    unsigned hashPart1 = sha1Hash[orderNumber % HASH_SIZE];
    unsigned hashPart2Sha1[HASH_SIZE];
    unsigned password = 1;

    hashPart1 = (hashPart1 + orderNumber)
            % (unsigned) (pow(10, PASSWORD_LENGHT));

    callHashFunction(hashPart1, hashPart2Sha1);

    cudaMakeSumOfHash(hashPart2Sha1);

    password = (hashPart2Sha1[0]) %
            (unsigned) (pow(10, PASSWORD_LENGHT));

    return password;
}

void ChainsGenerator::printSha1(unsigned *hesh) {
    int i;
    for (i = 0; i < 5; i++) {
        printf("%u ", hesh[i]);
    }
}

void ChainsGenerator::printSha1Hex(unsigned *hesh) {
    int i;
    for (i = 0; i < 5; i++) {
        printf("%0*x ", 8, hesh[i]);
    }
}
