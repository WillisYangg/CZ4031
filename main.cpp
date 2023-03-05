#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <set>
#include <stdio.h>
#include <limits.h>
#include <map>
#include <cmath>
#include <sstream>
#include "storage.h"
#include <fstream>
#include "bplustree.h"

int main(){
    Storage* storage = new Storage();
    storage->store_data("data.tsv");
    BPlusTree bplustree;
    bplustree.createTreeFromStorage(storage);
    std::cout << "Start Test" << std::endl;
    // bplustree.display();
    // bplustree.displayRecords(storage);
    storage->experiment1();
    std::cout << std::endl;
    bplustree.experiment2();
    std::cout << std::endl;
    bplustree.experiment3(500,storage);
    std::cout << std::endl;
    bplustree.experiment4(30000,40000, storage);
    std::cout << std::endl;
    bplustree.experiment5(1000, storage);
    bplustree.display();
    std::cout << "End Test" << std::endl;
}