#include <iostream>
#include <climits>
#include <fstream>

using namespace std;

int MAX;

//BP Node
class Node 
{
    bool IS_LEAF;
    int *key, size;
    Node **ptr;
    friend class BPlusTree;
public:
    Node();
};

class BPlusTree
{
    Node *root; //root node
    void insertInternal(int, Node *, Node *);
    Node *findParent(Node *, Node *);

public:
    BPlusTree();
    void search(int x);
    void insert(int x);
    void display(Node *);
    Node* getRoot();
};

//Node() initialisation
Node::Node()
{
    key = new int[MAX];
    ptr = new Node *[MAX +1];
};

//BPlusTree() initialisation
BPlusTree::BPlusTree()
{
    root=NULL;
};

//insert operation logic
void BPlusTree::insert(int x)
{   
    //empty tree
    if (root==NULL)
    {
        root = new Node;
        root->key[0] = x;
        root->IS_LEAF = true;
        root->size = 1;
    }
    else
    {

    }
};

//getRoot operation logic
Node *BPlusTree::getRoot()
{
    return root;
}