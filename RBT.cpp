//============================================================================
// Name        : RBT.cpp
// Author      : petr
// Version     :
// Copyright   : free
// Description : Rainbow table
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "RainbowTable.h"

int main(int argc, char **argv) {
    puts("Rainbow tables.");

    if (argc < 2) {
        puts("Use following keys: "
            "\n\t \"generate 100\" - generate table with 100 records"
            "\n\t\t (chain length predefined as 1000)"
            "\n\t \"clean\" - clean table"
            "\n\t \"print\" - print table"
            "\n\t \"find hash_sha1\" - find password for hash hash_sha1"
            "\n\t \"test\" - run test example"
            "\n\t\t (generates tables and find password of some hash)"
            "");
        return EXIT_SUCCESS;
    }

    RainbowTable dataTable;
    time_t t1;
    time_t t2;

    if (argc == 3 && strcmp(argv[1], "generate") == 0) {
        unsigned generateTableRecordsCount = atoi(argv[2]);

        dataTable.cleanTable();

        t1 = time(NULL);

        dataTable.generateTable(generateTableRecordsCount);

        t2 = time(NULL);
        cout << t2 - t1 << " seconds elapsed" << endl;

    } else if (argc == 2 && strcmp(argv[1], "clean") == 0) {

        dataTable.cleanTable();

    } else if (argc == 3 && strcmp(argv[1], "find") == 0) {

        t1 = time(NULL);

        printf("Password: %u\n", dataTable.findPasswordByHash(argv[2]));

        t2 = time(NULL);
        cout << t2 - t1 << " seconds elapsed" << endl;

    } else if (argc == 2 && strcmp(argv[1], "print") == 0) {
        dataTable.print();
    } else if (argc == 2 && strcmp(argv[1], "test") == 0) {
        unsigned generateTableRecordsCount = 60;
        char hash[41] = "1bbc81a994daa9a30a5dfa922eebd63d3e5a461d";

        t1 = time(NULL);

        dataTable.cleanTable();
        dataTable.generateTable(generateTableRecordsCount);

        t2 = time(NULL);
        cout << t2 - t1 << " seconds elapsed" << endl;

        t1 = time(NULL);

        printf("Password: %u\n", dataTable.findPasswordByHash(hash));

        t2 = time(NULL);
        cout << t2 - t1 << " seconds elapsed" << endl;
    }

    return EXIT_SUCCESS;
}

