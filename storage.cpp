#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <set>
#include <stdio.h>
#include <limits.h>
#include <map>

// #include <bits/stdc++.h>
#include <cmath>
#include <sstream>
#include "storage.h"
#include <fstream>
#include <chrono>

using namespace std::chrono;

bool compareByNumVotes(const Record& a, const Record& b) {
    return a.numVotes < b.numVotes;
}

void Storage::test(){
    std::cout << "Testing" << std::endl;
}

void Storage::store_data(string file) {
    
    std::ifstream movieData(file);
    std::string line;

    string tconst;
    string rating;
    string numVotes;
    float rating_float;
    std::vector<Record> records;
    unsigned char record[record_size];
    unsigned char *curPtr = basePtr;
    this->numBlocks = 1;
    std::map<unsigned char *, unsigned int> map;
    int no_of_records_in_block = 0;

    // skip first row
    getline(movieData, line);
    // read line by line
    while (std::getline(movieData, line)) {
        Record record_obj;
        std::istringstream ss(line);
        // read each element by delimiter
        std::getline(ss, tconst, '\t');
        std::getline(ss, rating, '\t');
        std::getline(ss, numVotes, '\t');

        std::stringstream(rating) >> rating_float;
        record_obj.tconst = tconst;
        // rating is now an integer that is  out of 100 instead of out of 10
        //since 100 < 256, it can be stored in 1byte
        record_obj.rating = rating_float*10;
        record_obj.numVotes = stoi(numVotes);
        records.push_back(record_obj);
    }
    std::sort(records.begin(), records.end(), compareByNumVotes);
    for(Record record_obj : records){
        // check if block has space, if not move on to the next block
        if (no_of_records_in_block == max_records_per_block) {
            no_of_records_in_block = 0;
            curPtr += this->excess_block_size;
            this->numBlocks++;
        }

        //reset record to store new incoming record
        memset(record, '\0', record_size);
        int index = 0;
        //store tconst in record
        for (unsigned char c : record_obj.tconst) {
            record[index] = c;
            index++;
        }
        //store rating in record
        record[tconst_size] = record_obj.rating;
        index = tconst_size + rating_size;
        //store numVotes in record
        for (int i = 0; i < sizeof(int); i++) {
            unsigned char *p = convertIntToBytes(record_obj.numVotes);
            record[index + i] = *(p + i);
        }
        //store record into storage
        for (int i = 0; i < record_size; i++) {
            curPtr[i] = record[i];
        }
        used_storage_size += record_size;
        curPtr += record_size;
        no_of_records_in_block ++;
        numRecords++;
    }
}


unsigned int Storage::retrieve_record_votes(unsigned char* curPtr){
    return convertBytesToInt(curPtr+ tconst_size + rating_size);
}

unsigned int Storage::retrieve_block_id(unsigned char* curPtr){
    return (int)(curPtr - basePtr)/block_size +1;
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
        std::cout <<"\t";
        std::cout << (float)*curPtr/10 << "\t";
        curPtr++;
        std::cout << convertBytesToInt(curPtr) << std::endl;
        curPtr+=4;
    }
}

void Storage::display_all_records(){
    unsigned char *curPtr = basePtr;
    int curRecord = 0;
    int block_no = 1;
    while(curRecord < numRecords){
        if (curRecord%max_records_per_block == 0) std::cout << "Block: " << block_no << std::endl;
        display_record(curPtr);
        curRecord++;
        curPtr +=record_size;
        if (curRecord%max_records_per_block == 0) {
            curPtr += excess_block_size;
            block_no++;
            std::cout << std::endl;
        }
    }
}

void Storage::experiment1(){
    std::cout<< "Number of Records: " << numRecords << std::endl;
    std::cout<< "Record Size: " << record_size << "bytes" <<std::endl;
    std::cout<< "Number of Records in a Block: " << max_records_per_block << std::endl;
    std::cout<< "Number of Blocks: " << numBlocks << std::endl;
}

// brute force linear scan search operation
void Storage::experiment3(int x){
    unsigned char *curPtr = basePtr;
    int block_no = 1;
    int curRecord = 0; 
    int reached = 0;
    
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    // parse through if not x yet
    while (retrieve_record_votes(curPtr) <= x){
        // storage ordered in terms of numVotes, parse through to check if x-1 has records of x
        if (retrieve_record_votes(curPtr) == x-1){
          curPtr += record_size;
          curRecord++; 
          if (curRecord%max_records_per_block == 0) {
                curPtr += excess_block_size;
                block_no++;
                std::cout << std::endl;
            }
        }
        // retrieve record if x
        if (retrieve_record_votes(curPtr) == x){    
            if (curRecord%max_records_per_block == 0 || reached == 0) std::cout << "Block: " << block_no << std::endl;
            display_record(curPtr); 
            curRecord++;
            reached++;
            curPtr += record_size;
            // reached end of block, next block
            if (curRecord%max_records_per_block == 0) {
                curPtr += excess_block_size;
                block_no++;
                std::cout << std::endl;
            }
        }
        // not x, go to next block
        else{
            curPtr += block_size;
            block_no++;
        }
    }
    // Get ending timepoint
    auto stop = high_resolution_clock::now();
    // Get duration. Substart timepoints to
    // get duration. To cast it to proper unit
    // use duration cast method
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Number of data blocks accessed by linear scan: " << block_no << endl;
    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
}

// brute force linear scan ranged search operation
void Storage::experiment4(int x, int y){
    unsigned char *curPtr = basePtr;
    int block_no = 1;
    int curRecord = 0; 
    int reached = 0;
    
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    // parse through if not x yet
    while (retrieve_record_votes(curPtr) <= y ){
        // storage ordered in terms of numVotes, parse through to check if x-1 has records of x
        if (retrieve_record_votes(curPtr) == x-1){
          curPtr += record_size;
          curRecord++; 
          if (curRecord%max_records_per_block == 0) {
                curPtr += excess_block_size;
                block_no++;
                std::cout << std::endl;
            }
        }
        // retrieve record ranging from x to y 
        if (retrieve_record_votes(curPtr) >= x && retrieve_record_votes(curPtr) <= y){    
            if (curRecord%max_records_per_block == 0 || reached == 0) std::cout << "Block: " << block_no << std::endl;
            display_record(curPtr); 
            curRecord++;
            reached++;
            curPtr += record_size;
            // reached end of block, next block
            if (curRecord%max_records_per_block == 0) {
                curPtr += excess_block_size;
                block_no++;
                std::cout << std::endl;
            }
        }
        // not in range, go to next block
        else{
            curPtr += block_size;
            block_no++;
        }
    }
    // Get ending timepoint
    auto stop = high_resolution_clock::now();
    // Get duration. Substart timepoints to
    // get duration. To cast it to proper unit
    // use duration cast method
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Number of data blocks accessed by linear scan: " << block_no << endl;
    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
}

// int main(){
//     Storage* storage = new Storage();
//     storage->store_data();
//     storage->display_all_records();
//     std::cout << std::endl;
//     storage->experiment1();
// }