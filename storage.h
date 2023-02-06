#include <iostream>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <limits.h>
// #include <bits/stdc++.h>
#include <cmath>
using namespace std;

class Storage{
public:
    //100MB
    int storage_size = 100*1024*1024;
    unsigned char *base_ptr = (unsigned char*) malloc(storage_size);
    int used_storage_size = 0;
    //200B
    int block_size = 200;
    //10byte for tconst
    //1byte for rating 
    //4bytes for votes
    int record_size = 15;

    Storage(){
    }

    ~Storage(){
        free(this -> base_ptr);
    }

    //loadData
    void test();

};