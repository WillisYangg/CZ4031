#include <iostream>
#include <climits>
#include <fstream>
#include <sstream>

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
        Node *cursor = root;
        Node *parent;
        //To travel to the leaf node
        while (cursor->IS_LEAF == false)
        {
            parent = cursor;
            for (int i=0;i<cursor->size;i++)
            {
                if (x<cursor->key[i])
                {
                    cursor = cursor->ptr[i];
                    break;
                }
                if (i == cursor->size-1)
                {
                    cursor = cursor->ptr[i+1];
                    break;
                }
            }
        }
        //cursor at leaf node, add nodes to the leaf node
        if (cursor->size < MAX)
        {
            int i = 0;
            while (x > cursor->key[i] && i < cursor->size)
            {
                i++;
            }
            for (int j = cursor->size; j>1; j--)
            {
                cursor->key[j] = cursor->key[j-1]; //for the new key
            }
            cursor->key[i] = x;
            cursor->size++;
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
            cursor->ptr[cursor->size - 1] = NULL;
            cout<<"Inserted "<< x <<" successfully\n";
        }
        //create a new leaf node if it is already at the max
        else
        {
            cout<<"Leaf node has reached, creating new leaf node\n";
            //create new leaf node
            Node *newLeaf = new Node;
            int virtualNode[MAX+1];
            for (int i=0;i<MAX; i++)
            {
                virtualNode[i] = cursor->key[i];
            }
            int i =0,j;
            for (int j = MAX + 1; j > i; j--) 
            {
                virtualNode[j] = virtualNode[j - 1];
            }
            virtualNode[i] = x;
            newLeaf->IS_LEAF = true;
            //split the cursor
            cursor->size = (MAX + 1) / 2;
            newLeaf->size = MAX + 1 - (MAX + 1) / 2;
            //cursor point to the new leaf that just got created
            cursor->ptr[cursor->size] = newLeaf;
            //new leafNode point to the the new leaf that just got created
            newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
            cursor->ptr[MAX] = NULL;
            //fill up the new leaf node
            for (i=0;i<cursor->size;i++)
            {
                cursor->key[i] = virtualNode[i];
            }
            for (i=0, j=cursor->size; i<newLeaf->size; i++,j++)
            {
                newLeaf->key[i] = virtualNode[j];
            }
            //tree only has one node (root node)
            if (cursor == root)
            {
                //create a new root node
                //tree grows upwards, form new root node
                Node *newRootNode = new Node();
                newRootNode->key[0] = newLeaf->key[0];
                newRootNode->ptr[0] = cursor;
                newRootNode->ptr[1] = newLeaf;
                newRootNode->IS_LEAF = false;
                newRootNode->size = 1;
                root = newRootNode;
                cout<<"Inserted new root node successfully\n";
            }
            else
            {
                //insert new key into parent node
                insertInternal(newLeaf->key[0], parent, newLeaf);
                cout<<"Inserted key into parent node successfully\n";
            }
        }
    }
};

//getRoot operation logic
Node *BPlusTree::getRoot()
{
    return root;
}