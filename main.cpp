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
#include "bplustree.h"

int main(){
    Storage* storage = new Storage();
    storage->store_data();
    // storage->display_all_records();
    std::cout << std::endl;
    storage->experiment1();

    // BPlusTree* bplustree = new BPlusTree();
    BPlusTree bplustree;
    bplustree.createTreeFromStorage(storage);
    std::cout << "testing" << std::endl;
    bplustree.display(bplustree.getRoot());
}