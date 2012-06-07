/*
 * RainbowTable.h
 *
 *  Created on: May 13, 2012
 *      Author: petr
 */

#include <string.h>
#include <sstream>
#include "Database/Database.h"
#include "ChainsGenerator.h"
#include "threadpool/threadpool.h"

#define THREADS_COUNT 20

#ifndef DATATABLE_H_
#define DATATABLE_H_

#define PATH_TO_BD "Database/db.sl3"

class ChainGeneratingWorkerThread: public WorkerThread {
public:
    int id;

    unsigned virtual executeThis();
    ChainGeneratingWorkerThread(int id);
    ~ChainGeneratingWorkerThread();
};

class PasswordFindWorkerThread: public WorkerThread {
public:
    int id;
    unsigned* hash;
    unsigned static result;
    unsigned static falseAlarmCount;

    unsigned virtual executeThis();
    PasswordFindWorkerThread(int id, unsigned* hash);
    ~PasswordFindWorkerThread();
};


class RainbowTable {
public:
    RainbowTable();

    virtual ~RainbowTable();

    static ChainsGenerator chainsGenerator;

    void static generateChain();

    void init();

    void cleanTable();

    void static addChain( char* chain_head, char* chain_tail);

    bool static findChain( char* chain_head, char* chain_tail );

    vector<vector<string> > static findChainTail( char* chain_tail );

    void print();

    void generateTable( unsigned generateTableRecordsCount );

    unsigned static findPassword( int i, unsigned* hash );

    unsigned findPasswordByHash( unsigned* hash );

    unsigned findPasswordByHash( char* hash );

private:
    static Database *db;
};





#endif /* DATATABLE_H_ */
