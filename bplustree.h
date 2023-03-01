#include <iostream>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <limits.h>
// #include <bits/stdc++.h>
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
    bool IS_LEAF;
    int *key;
    int size;
    Node **ptr;
    friend class BPlusTree;
public:
    Node();
};

class BPlusTree
{
    Node *root; //root node
    void insertInternal(int x, Node *, Node *);
    void removeInternal(int x, Node *, Node *);
    Node *findParent(Node *, Node *);

public:
    BPlusTree();
    void search(int x);
    void insert(int x);
    void remove(int x);
    void display(Node *);
    Node* getRoot();
    void createTreeFromStorage(Storage *storage);
};
#endif