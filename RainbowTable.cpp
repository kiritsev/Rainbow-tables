/*
 * RainbowTable.cpp
 *
 *  Created on: May 13, 2012
 *      Author: petr
 */

#include <iostream>
#include <string.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include "RainbowTable.h"
#include "ChainsGenerator.h"

ChainGeneratingWorkerThread::ChainGeneratingWorkerThread(int id)
    : WorkerThread(id), id(id) {
}

unsigned ChainGeneratingWorkerThread::executeThis() {
    RainbowTable::generateChain();
    return 0;
};

ChainGeneratingWorkerThread::~ChainGeneratingWorkerThread(){
};

PasswordFindWorkerThread::PasswordFindWorkerThread(int id, unsigned* hash)
    : WorkerThread(id), id(id), hash(hash) {
};

unsigned PasswordFindWorkerThread::executeThis() {
        unsigned result;
        if ( (result = RainbowTable::findPassword(id,hash)) != 0 ) {
            PasswordFindWorkerThread::result = result;
        }
        return 0;
};

PasswordFindWorkerThread::~PasswordFindWorkerThread(){
};

unsigned PasswordFindWorkerThread::result = 0;

unsigned PasswordFindWorkerThread::falseAlarmCount = 0;

Database *RainbowTable::db;

RainbowTable::RainbowTable() {
    RainbowTable::init();
}

RainbowTable::~RainbowTable() {
    db->close();
    delete (db);
    puts("DB closed!");
}

void RainbowTable::init() {
    puts("Init DB.");

    const char* crate_rb_table_query = ""
            "CREATE TABLE IF NOT EXISTS rainbow_table ("
            "    chain_head varchar(16),"
            "    chain_tail varchar(16)"
            ");";

    db = new Database((char*) PATH_TO_BD);

    db->query((char*) crate_rb_table_query);
}

void RainbowTable::cleanTable() {
    puts("Clean table");
    db->query((char*) "DELETE FROM rainbow_table;");
}

void RainbowTable::addChain(char* chain_head, char* chain_tail) {
    char query[1000];
    sprintf(query, "INSERT INTO rainbow_table"
            " VALUES('%s', '%s');",
            chain_head,    chain_tail);
    db->query(query);
}

bool RainbowTable::findChain(char* chain_head, char* chain_tail) {
    char query[1000];
    vector<vector<string> > result;

    sprintf(query,
            "SELECT * FROM rainbow_table "
            " WHERE chain_head = '%s' "
            "    AND chain_tail = '%s' ;",
            chain_head, chain_tail);
    result = db->query(query);
    return result.size() > 0;
}

vector<vector<string> > RainbowTable::findChainTail(char* chain_tail) {
    char query[1000];
    sprintf(query, "SELECT * FROM rainbow_table "
            " WHERE chain_tail = '%s' ;",
            chain_tail);
    return db->query(query);
}

void RainbowTable::print() {
    vector<vector<string> > result = db->query(
            (char*) "SELECT chain_head, chain_tail"
            " FROM rainbow_table;");
    for (vector<vector<string> >::iterator it = result.begin();
            it < result.end(); ++it) {
        vector<string> row = *it;
        std::cout << "[" << row.at(0) << ", "
                << row.at(1) << "]" << std::endl;
    }
}

void RainbowTable::generateChain() {
    Chain chain;
    unsigned password;

    password = chainsGenerator.generateRandomPassword();
    sprintf(chain.head, "%u", password);
    chainsGenerator.generateChain(&chain);

    //printf("chain %i, head: %s, tail: %s \n",
    //i, chain.head, chain.tail);

    if (!findChain(chain.head, chain.tail)) {
        addChain(chain.head, chain.tail);
    }
}

ChainsGenerator RainbowTable::chainsGenerator;

void RainbowTable::generateTable(unsigned generateTableRecordsCount) {
    puts("Generate Table::start");

    unsigned i;

    ThreadPool* myPool = new ThreadPool(THREADS_COUNT);
    myPool->initializeThreads();

    for (i = 0; i < generateTableRecordsCount; i++) {
        ChainGeneratingWorkerThread* myThread =
                new ChainGeneratingWorkerThread(i);
        myPool->assignWork(myThread);
    }

    myPool->destroyPool(1);
    delete myPool;

    puts("Generate Table::finish");
}

unsigned RainbowTable::findPassword(int i, unsigned* hash) {
    int j;
    Chain resultChain;
    vector<vector<string> > result;
    unsigned password = 0;

    ChainsGenerator::generateChainFromPosition(i, hash, &resultChain);

    result = findChainTail(resultChain.tail);

    if (result.size() != 0) {
        for (j = 0; j < (int) result.size(); ++j) {
            password = ChainsGenerator::generateChainUntilAimHash(
                    atoi(result.at(j).at(0).c_str()), hash);
            if (password != 0) {
                return password;
            }
        }
        PasswordFindWorkerThread::falseAlarmCount++;
    }

    return 0;
}

unsigned RainbowTable::findPasswordByHash(char* hash) {
    unsigned blockUnsigned[HASH_SIZE];
    char blockChar[HASH_BLOCK_CHARACTERS_SIZE + 1];

    blockChar[HASH_BLOCK_CHARACTERS_SIZE] = '\0';

    printf("Hash: %s\n", hash);

    for (int i = 0; i < HASH_SIZE; ++i) {
        strncpy(blockChar, hash + i * HASH_BLOCK_CHARACTERS_SIZE,
                HASH_BLOCK_CHARACTERS_SIZE);

        sscanf(blockChar, "%x", &(blockUnsigned[i]));
    }

    return findPasswordByHash(blockUnsigned);
}

unsigned RainbowTable::findPasswordByHash(unsigned* hash) {
    int i;
    Chain resultChain;
    vector<vector<string> > result;

    ThreadPool* myPool = new ThreadPool(THREADS_COUNT);
    myPool->initializeThreads();

    for (i = CHAIN_LENGTH - 1; i >= 0; --i) {

        PasswordFindWorkerThread* myThread =
                new PasswordFindWorkerThread(i, hash);
        myPool->assignWork(myThread);

        if (PasswordFindWorkerThread::result != 0) {
            break;
        }

    }

    myPool->destroyPool(1);
    delete myPool;

    printf("false alarm count: %d\n",
            PasswordFindWorkerThread::falseAlarmCount);

    if (PasswordFindWorkerThread::result != 0) {
        return PasswordFindWorkerThread::result;
    }

    return 0;
}
