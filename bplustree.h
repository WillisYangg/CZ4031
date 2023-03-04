#include <iostream>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <limits.h>
#include <cmath>
#include "storage.h"
using namespace std;
#ifndef BPLUSTREE_H
#define BPLUSTREE_H
// struct keys_struct
// {
//   float key_value;
//   vector <unsigned char*> add_vect;
// };

// //BP Node
class Node 
{
    bool IS_LEAF; //2byte
    int *key; //8byte
    int size; //4byte
    Node **ptr;//8byte
    //to point to records
    unsigned char **records;//8byte
    friend class BPlusTree;
public:
    Node();
};

class BPlusTree
{
    int nodes = 0;
    int levels = 0;
    int numKeys = 0;
    Node *root; //root node
    void insertInternal(int x, Node *, Node *);
    void deleteInternal(int x, Node *, Node *);
    Node *findParent(Node* curNode, Node *child);

public:
    BPlusTree();
    void search(int x);
    void insert(int x,unsigned char *record);
    void deleteKey(int x);
    void display();
    void displayRecords(Storage *storage);
    void createTreeFromStorage(Storage *storage);
    Node* createNewLeafNode(int x, unsigned char *record);
    Node* createNewBufferNode(int x, unsigned char *record);
    Node** traverseToLeafNode(int x);

    void experiment2();
    void experiment3(int x, Storage *storage);
    void experiment4(int x, int y, Storage *storage);
    void experiment5(int x, Storage *storage);
};
#endif