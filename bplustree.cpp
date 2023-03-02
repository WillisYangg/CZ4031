#include <iostream>
#include <climits>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "bplustree.h"

using namespace std;

const int MAX = 3;

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
    cout<<"Inserting "<< x <<" now\n";
    //empty tree
    if (root==NULL)
    {
        root = new Node;
        root->key[0] = x;
        root->IS_LEAF = true;
        root->size = 1;
        cout<<"Root Node:  "<< x << endl;
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
            cout<<"1\n";
            //tree only has one node (root node)
            if (cursor == root)
            {
                cout<<"2\n";
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
                cout<<"3\n";
                //insert new key into parent node
                insertInternal(newLeaf->key[0], parent, newLeaf);
                cout<<"Inserted key into parent node successfully\n";
            }
        }
    }
}

//insertInternal operation logic
void BPlusTree::insertInternal(int x, Node *cursor, Node *child)
{
    //cursor is not full
    cout<<"a\n";
    if (cursor->size <  MAX)
    {
        cout<<"b\n";
        int i = 0;
        while (x > cursor->key[i] && i < cursor->size)
        {
            i++;
        }
        //new key
        for (int j=cursor->size;j>i;j--)
        {
            cursor->key[j] = cursor->key[j-1];
        }
        //new pointer
        for (int j=cursor->size+1;j>i+1;j--)
        {
            cursor->ptr[j] = cursor->ptr[j-1];
        }
        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[i+1] = child;
        cout<<"Inserted key into internal node successfully\n";
    }
    else
    {
        cout<<"c\n";
        //create new internal node
        Node *newInternalNode = new Node();
        int virtualKey[MAX+1];
        Node *virtualPtr[MAX+2];
        //new key
        for (int i=0; i<MAX;i++)
        {
            virtualKey[i] = cursor->key[i];
        }
        //new pointer
        for (int i=0; i<MAX+1;i++)
        {
            virtualPtr[i] = cursor->ptr[i];
        }
        int i=0, j;
        while (x > virtualKey[i] && i<MAX)
        {
            i++;
        }
        for (int j = MAX+1; j>i; j--)
        {
            virtualKey[j] = virtualKey[j-1];
        }
        virtualKey[i] = x;
        for (int j=MAX+2; j>i; j--)
        {
            virtualPtr[j] = virtualPtr[j-1];
        }
        virtualPtr[i+1]=child;
        newInternalNode->IS_LEAF = false;
        cursor->size = (MAX+1)/2;
        newInternalNode->size = MAX - (MAX+1)/2;
        //fill up the new internal node
        for (i=0, j=cursor->size+1; i<newInternalNode->size; i++,j++)
        {
            newInternalNode->key[i] = virtualKey[j];
        }
        for (i=0, j=cursor->size+1; i<newInternalNode->size+1; i++,j++)
        {
            newInternalNode->ptr[i] = virtualPtr[j];
        }
        if (cursor == root)
        {
            cout<<"d\n";
            //create a new root node
            Node *newRootNode = new Node();
            newRootNode->key[0] = cursor->key[cursor->size];
            newRootNode->ptr[0] = cursor;
            newRootNode->ptr[1] = newInternalNode;
            newRootNode->IS_LEAF = false;
            newRootNode->size = 1;
            root = newRootNode;
            cout<<"Inserted new root node successfully\n";
        }
        else
        {
            cout<<"e\n";
            insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternalNode);
        }
    }
}

//findParent operation logic
Node *BPlusTree::findParent(Node* cursor, Node* child)
{
    Node *parent;
    if (cursor->IS_LEAF || (cursor->ptr[0]->IS_LEAF))
    {
        return NULL;
    }
    for (int i=0; i<cursor->size; i++)
    {
        if (cursor->ptr[i] == child)
        {
            parent = cursor;
            return parent;
        }
        else
        {
            parent = findParent(cursor->ptr[i], child);
            if (parent != NULL)
            {
                return parent;
            }
        }
    }
    return parent;
}

//getRoot operation logic
Node *BPlusTree::getRoot()
{
    return root;
}

//remove operational logic
void BPlusTree::remove(int x) {
  if (root == NULL) {
    cout << "Tree empty\n";
  } else {
    Node *cursor = root;
    Node *parent;
    int leftSibling, rightSibling;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->size; i++) {
        parent = cursor;
        leftSibling = i - 1;
        rightSibling = i + 1;
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          leftSibling = i;
          rightSibling = i + 2;
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    bool found = false;
    int pos;
    for (pos = 0; pos < cursor->size; pos++) {
      if (cursor->key[pos] == x) {
        found = true;
        break;
      }
    }
    if (!found) {
      cout << "Not found\n";
      return;
    }
    for (int i = pos; i < cursor->size; i++) {
      cursor->key[i] = cursor->key[i + 1];
    }
    cursor->size--;
    if (cursor == root) {
      for (int i = 0; i < MAX + 1; i++) {
        cursor->ptr[i] = NULL;
      }
      if (cursor->size == 0) {
        cout << "Tree died\n";
        delete[] cursor->key;
        delete[] cursor->ptr;
        delete cursor;
        root = NULL;
      }
      return;
    }
    cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];
    cursor->ptr[cursor->size + 1] = NULL;
    if (cursor->size >= (MAX + 1) / 2) {
      return;
    }
    if (leftSibling >= 0) {
      Node *leftNode = parent->ptr[leftSibling];
      if (leftNode->size >= (MAX + 1) / 2 + 1) {
        for (int i = cursor->size; i > 0; i--) {
          cursor->key[i] = cursor->key[i - 1];
        }
        cursor->size++;
        cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
        cursor->ptr[cursor->size - 1] = NULL;
        cursor->key[0] = leftNode->key[leftNode->size - 1];
        leftNode->size--;
        leftNode->ptr[leftNode->size] = cursor;
        leftNode->ptr[leftNode->size + 1] = NULL;
        parent->key[leftSibling] = cursor->key[0];
        return;
      }
    }
    if (rightSibling <= parent->size) {
      Node *rightNode = parent->ptr[rightSibling];
      if (rightNode->size >= (MAX + 1) / 2 + 1) {
        cursor->size++;
        cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
        cursor->ptr[cursor->size - 1] = NULL;
        cursor->key[cursor->size - 1] = rightNode->key[0];
        rightNode->size--;
        rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size + 1];
        rightNode->ptr[rightNode->size + 1] = NULL;
        for (int i = 0; i < rightNode->size; i++) {
          rightNode->key[i] = rightNode->key[i + 1];
        }
        parent->key[rightSibling - 1] = rightNode->key[0];
        return;
      }
    }
    if (leftSibling >= 0) {
      Node *leftNode = parent->ptr[leftSibling];
      for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++) {
        leftNode->key[i] = cursor->key[j];
      }
      leftNode->ptr[leftNode->size] = NULL;
      leftNode->size += cursor->size;
      leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];
      removeInternal(parent->key[leftSibling], parent, cursor);
      delete[] cursor->key;
      delete[] cursor->ptr;
      delete cursor;
    } else if (rightSibling <= parent->size) {
      Node *rightNode = parent->ptr[rightSibling];
      for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++) {
        cursor->key[i] = rightNode->key[j];
      }
      cursor->ptr[cursor->size] = NULL;
      cursor->size += rightNode->size;
      cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
      cout << "Merging two leaf nodes\n";
      removeInternal(parent->key[rightSibling - 1], parent, rightNode);
      delete[] rightNode->key;
      delete[] rightNode->ptr;
      delete rightNode;
    }
  }
}

//removeInternal operational logic
void BPlusTree::removeInternal(int x, Node *cursor, Node *child) {
  if (cursor == root) {
    if (cursor->size == 1) {
      if (cursor->ptr[1] == child) {
        delete[] child->key;
        delete[] child->ptr;
        delete child;
        root = cursor->ptr[0];
        delete[] cursor->key;
        delete[] cursor->ptr;
        delete cursor;
        cout << "Changed root node\n";
        return;
      } else if (cursor->ptr[0] == child) {
        delete[] child->key;
        delete[] child->ptr;
        delete child;
        root = cursor->ptr[1];
        delete[] cursor->key;
        delete[] cursor->ptr;
        delete cursor;
        cout << "Changed root node\n";
        return;
      }
    }
  }
  int pos;
  for (pos = 0; pos < cursor->size; pos++) {
    if (cursor->key[pos] == x) {
      break;
    }
  }
  for (int i = pos; i < cursor->size; i++) {
    cursor->key[i] = cursor->key[i + 1];
  }
  for (pos = 0; pos < cursor->size + 1; pos++) {
    if (cursor->ptr[pos] == child) {
      break;
    }
  }
  for (int i = pos; i < cursor->size + 1; i++) {
    cursor->ptr[i] = cursor->ptr[i + 1];
  }
  cursor->size--;
  if (cursor->size >= (MAX + 1) / 2 - 1) {
    return;
  }
  if (cursor == root)
    return;
  Node *parent = findParent(root, cursor);
  int leftSibling, rightSibling;
  for (pos = 0; pos < parent->size + 1; pos++) {
    if (parent->ptr[pos] == cursor) {
      leftSibling = pos - 1;
      rightSibling = pos + 1;
      break;
    }
  }
  if (leftSibling >= 0) {
    Node *leftNode = parent->ptr[leftSibling];
    if (leftNode->size >= (MAX + 1) / 2) {
      for (int i = cursor->size; i > 0; i--) {
        cursor->key[i] = cursor->key[i - 1];
      }
      cursor->key[0] = parent->key[leftSibling];
      parent->key[leftSibling] = leftNode->key[leftNode->size - 1];
      for (int i = cursor->size + 1; i > 0; i--) {
        cursor->ptr[i] = cursor->ptr[i - 1];
      }
      cursor->ptr[0] = leftNode->ptr[leftNode->size];
      cursor->size++;
      leftNode->size--;
      return;
    }
  }
  if (rightSibling <= parent->size) {
    Node *rightNode = parent->ptr[rightSibling];
    if (rightNode->size >= (MAX + 1) / 2) {
      cursor->key[cursor->size] = parent->key[pos];
      parent->key[pos] = rightNode->key[0];
      for (int i = 0; i < rightNode->size - 1; i++) {
        rightNode->key[i] = rightNode->key[i + 1];
      }
      cursor->ptr[cursor->size + 1] = rightNode->ptr[0];
      for (int i = 0; i < rightNode->size; ++i) {
        rightNode->ptr[i] = rightNode->ptr[i + 1];
      }
      cursor->size++;
      rightNode->size--;
      return;
    }
  }
  if (leftSibling >= 0) {
    Node *leftNode = parent->ptr[leftSibling];
    leftNode->key[leftNode->size] = parent->key[leftSibling];
    for (int i = leftNode->size + 1, j = 0; j < cursor->size; j++) {
      leftNode->key[i] = cursor->key[j];
    }
    for (int i = leftNode->size + 1, j = 0; j < cursor->size + 1; j++) {
      leftNode->ptr[i] = cursor->ptr[j];
      cursor->ptr[j] = NULL;
    }
    leftNode->size += cursor->size + 1;
    cursor->size = 0;
    removeInternal(parent->key[leftSibling], parent, cursor);
  } else if (rightSibling <= parent->size) {
    Node *rightNode = parent->ptr[rightSibling];
    cursor->key[cursor->size] = parent->key[rightSibling - 1];
    for (int i = cursor->size + 1, j = 0; j < rightNode->size; j++) {
      cursor->key[i] = rightNode->key[j];
    }
    for (int i = cursor->size + 1, j = 0; j < rightNode->size + 1; j++) {
      cursor->ptr[i] = rightNode->ptr[j];
      rightNode->ptr[j] = NULL;
    }
    cursor->size += rightNode->size + 1;
    rightNode->size = 0;
    removeInternal(parent->key[rightSibling - 1], parent, rightNode);
  }
}

// Search operation
void BPlusTree::search(int x) {
  if (root == NULL) {
    cout << "Tree is empty\n";
  } else {
    Node *cursor = root;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    for (int i = 0; i < cursor->size; i++) {
      if (cursor->key[i] == x) {
        cout << "Found\n";
        return;
      }
    }
    cout << "Not found\n";
  }
}

void BPlusTree::createTreeFromStorage(Storage *storage){
  //iterate through records
  //each record in storage takes up 15bytes
  //the basePtr points to the first byte in the first record
  //as such, the first 10bytes represents tconst of the first record
  //the 11th byte represents the rating 
  //the 12th - 15th byte represents numVotes
  //the 16th byte represents the first byte in the second record
  //so on n so forth
  unsigned char *curPtr = storage->basePtr;
  int curRecord = 0;
  int block_no = 1;
  while(curRecord < storage->numRecords){
    // vector<int> x;
    //not sure what the add_vect is for
    // x.push_back(curPtr);

    //this stored the numVotes tof the current record in key_value
    // std::cout << storage->convertBytesToInt(curPtr+storage->tconst_size + storage->rating_size) << std::endl;
    //im assuming that x.key_value is supposed to store the numVotes of the current record
    this->insert(storage->convertBytesToInt(curPtr+storage->tconst_size + storage->rating_size));
    //INSERTION INTO THE BPLUSTREE CAN TAKE PLACE HERE
    // insert(x);

    //comment out this section of code to remove printing the record in terminal
    //please uncomment line 575-577 below if you comment out this section of code
    //section start//
    // if (curRecord%storage->max_records_per_block == 0) std::cout << "Block: " << block_no << std::endl;
    // if(*curPtr == '\0') std::cout << "Record does not exist" << std::endl;
    // else{
    //   for(int i = 0; i< storage->tconst_size;i++){
    //     if(*curPtr != '\0') std::cout << *curPtr;
    //     curPtr++;
    //   }
    //   std::cout <<"\t";
    //   std::cout << (float)*curPtr/10 << "\t";
    //   curPtr++;
    //   std::cout << storage->convertBytesToInt(curPtr) << std::endl;
    //   curPtr+=4;
    // }
    //section end

    //keeps track of number of records iterated
    //this is also used to track which block we are in
    curRecord++;
    
    //uncomment these 3 lines of code if you comment out the previous code section
    // storage->display_record(curPtr);
    //jumps to the first byte of the next record
    curPtr +=storage->record_size;


    //if curRecord%store->max_records_per_block is zero
    //it means that the current block is full
    if (curRecord%storage->max_records_per_block == 0) {
        //points to the first byte of the first record in the next block
        curPtr += storage->excess_block_size;
        block_no++;
        std::cout << std::endl;
    }
  }
}

void BPlusTree::display(Node *cursor)
{
  if (cursor != NULL) 
  {
    for (int i = 0; i < cursor->size; i++) 
    {
      cout << cursor->key[i] << "|";
    }
    cout << "\n";
    if (cursor->IS_LEAF != true) 
    {
      for (int i = 0; i < cursor->size + 1; i++) 
      {
        display(cursor->ptr[i]);
      }
    }
  }
}

// int main(){
//   BPlusTree node;
//   node.insert(4);
//   // node.display(node.getRoot());
//   node.insert(15);
//   // node.display(node.getRoot());
//   node.insert(25);
//   // node.display(node.getRoot());
//   node.insert(35);
//   node.display(node.getRoot());
//   // node.insert(45);
//   // node.display(node.getRoot());
//   // node.insert(55);
//   // node.display(node.getRoot());
//   // node.insert(40);
//   // node.display(node.getRoot());
//   // node.insert(30);
//   // node.display(node.getRoot());
//   // node.insert(20);
//   // node.display(node.getRoot());
// }