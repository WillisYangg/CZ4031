#include <iostream>
#include <cstring>
#include <vector>
#include <set>
#include <stdio.h>
#include <limits.h>
// #include <bits/stdc++.h>
#include <cmath>
#include "storage.h"

void Storage::test(){
    unsigned char* curPtr = base_ptr;
    //every index in curPtr is one byte
    //when loading data, it starts at base_ptr
    //make sure 1 block doesn not exceed 200B
    //means for record size = 15, only 13 records can be inserted into each block before moving onto next block
    //keep track of the base address for the start of each block to use as block pointers
    //base_ptr is the base address for the first block, base_ptr+200 points to block 2
        

}