#include <iostream>
#include <cstring>
#include <vector>
#include <set>
#include <stdio.h>
#include <limits.h>
#include <map>

// #include <bits/stdc++.h>
#include <cmath>
#include <sstream>
#include "storage.h"

void Storage::test(){
    string tconst = "t00000001";
    string adjustedRating = "5.9";
    string numVotes = "2413467";

    string tconst2 = "t00000002";
    string adjustedRating2 = "10.0";
    string numVotes2 = "6787234";

    float rating_float;
    unsigned char record[record_size];
    std::map<unsigned char*, unsigned int> map;
    
    memset(record, '\0', record_size);
    int index = 0;
    unsigned char *curPtr = basePtr + used_storage_size;
    for(unsigned char c: tconst){
        record[index] = c;
        index++;
    }
    std::stringstream (adjustedRating) >> rating_float;
    //rating is now an integer that is  out of 100 instead of out of 10
    record[tconst_size] = rating_float*10;
    index = tconst_size + rating_size;
    for(int i = 0; i <sizeof(int); i++){
        unsigned char *p = convertIntToBytes(stoi(numVotes));
        record[index+i] = *(p+i);
    }
    for(int i = 0; i<record_size;i++){
        curPtr[i] = record[i];
    }
    used_storage_size += record_size;

    memset(record, '\0', record_size);
    index = 0;
    curPtr = basePtr + used_storage_size;
    for(unsigned char c: tconst2){
        record[index] = c;
        index++;
    }
    std::stringstream (adjustedRating2) >> rating_float;
    //rating is now an integer that is  out of 100 instead of out of 10
    record[tconst_size] = rating_float*10;
    index = tconst_size + rating_size;
    for(int i = 0; i <sizeof(int); i++){
        unsigned char *p = convertIntToBytes(stoi(numVotes2));
        record[index+i] = *(p+i);
    }
    for(int i = 0; i<record_size;i++){
        curPtr[i] = record[i];
    }
    used_storage_size += record_size;

    std::cout << retrieve_record_votes(basePtr)<<std::endl;
    display_record(basePtr);
    display_record(curPtr);
    delete_record(basePtr);
    display_record(basePtr);
    display_record(curPtr);
    std::cout << sizeof(int) <<std::endl;
}

void Storage::store_data(){
    //make sure to check that each block can only contain 13 records
}

unsigned int Storage::retrieve_record_votes(unsigned char* curPtr){
    return convertBytesToInt(curPtr+ tconst_size + rating_size);
}

unsigned int Storage::retrieve_block_id(unsigned char* curPtr){
    return (int)(curPtr - basePtr)/block_size;
}

void Storage::delete_record(unsigned char* curPtr){
    for(int i = 0; i < record_size; i++){
        *curPtr = '\0';
        curPtr++; 
    }
}

unsigned char * Storage::convertIntToBytes(unsigned int n){
    unsigned char* bytes = (unsigned char*) malloc(sizeof(int));
    //rightshift 3bytes to convert first byte of integer
    bytes[0] = (n >> 24) & 0xFF;
    //rightshift 2bytes to convert second byte of integer
    bytes[1] = (n >> 16) & 0xFF;
    //rightshift 1byte to convert third byte of integer
    bytes[2] = (n >> 8) & 0xFF;
    //rightshift 0byte to convert fourth byte of integer
    bytes[3] = n & 0xFF;
    return bytes;
}

unsigned int Storage::convertBytesToInt(unsigned char * bytes){
    //left shifting to form back the 4 byte integer
    return bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3] << 0;
}


void Storage::display_record(unsigned char* curPtr){
    if(*curPtr == '\0') std::cout << "Record does not exist" << std::endl;
    else{
        for(int i = 0; i< tconst_size;i++){
            if(*curPtr != '\0') std::cout << *curPtr;
            curPtr++;
        }
        std::cout <<std::endl;
        std::cout << (float)*curPtr/10 << std::endl;
        curPtr++;
        std::cout << convertBytesToInt(curPtr) << std::endl;
        curPtr+=4;
    }
}

int main(){
    Storage* storage = new Storage();
    storage->test();
}