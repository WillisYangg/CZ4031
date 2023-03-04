#include <iostream>
#include <climits>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "bplustree.h"
#include <queue>
#include <set>
#include <chrono>

using namespace std;

// size of node = size of block = 200
// size of node = 2 +4N + 4 + 8(N+1)
// 200 = 12N +14
// N = (200-14)/12
const int N = (200 - 14) / 12;

// Node() initialisation
Node::Node()
{
  key = new int[N];
  ptr = new Node *[N + 1];
  records = new unsigned char *[N];
};

// BPlusTree() initialisation
BPlusTree::BPlusTree()
{
  root = NULL;
};

Node *BPlusTree::createNewLeafNode(int x, unsigned char *record)
{
  Node *node = new Node;
  node->key[0] = x;
  // node->records[0] = record;
  node->IS_LEAF = true;
  node->size = 1;

  node->ptr[0] = createNewBufferNode(x, record);
  node->ptr[N] = NULL;
  return node;
}

// returns [Node* parentOfLeafNode, Node *leafNode]
Node **BPlusTree::traverseToLeafNode(int x)
{
  Node **parent_child = new Node *[2];
  parent_child[1] = root;
  // To travel to the leaf node
  while (parent_child[1]->IS_LEAF == false)
  {
    parent_child[0] = parent_child[1];
    for (int i = 0; i < parent_child[1]->size; i++)
    {
      if (x < parent_child[1]->key[i])
      {
        parent_child[1] = parent_child[1]->ptr[i];
        break;
      }
    }
    if (parent_child[0] == parent_child[1])
      parent_child[1] = parent_child[1]->ptr[parent_child[1]->size];
  }
  return parent_child;
}
Node *BPlusTree::createNewBufferNode(int x, unsigned char *record)
{
  Node *node = new Node;
  node->records[0] = record;
  node->size = 1;
  node->key[0] = x;
  node->ptr[0] = NULL;
  return node;
}
// insert operation logic
void BPlusTree::insert(int x, unsigned char *record)
{
  // empty tree
  if (root == NULL)
  {
    root = createNewLeafNode(x, record);
    cout << "First root node inserted:  " << x << endl;
    this->nodes++;
    this->levels++;
    this->numKeys++;
  }
  else
  {
    Node *parent = traverseToLeafNode(x)[0];
    Node *curNode = traverseToLeafNode(x)[1];
    int insertIndex = 0;
    while (x > curNode->key[insertIndex] && insertIndex < curNode->size)
    {
      insertIndex++;
    }
    // account for duplicate keys
    if (x == curNode->key[insertIndex])
    {
      Node *curBuffer = curNode->ptr[insertIndex];
      // curBuffer should never be null in the first iteration
      while (true)
      {
        if (curBuffer->ptr[0] != NULL)
          curBuffer = curBuffer->ptr[0];
        else
          break;
      }
      int key_buffer_size = curBuffer->size;
      // there is remaining space in the buffer
      if (key_buffer_size < N)
      {
        curBuffer->records[curBuffer->size] = record;
        curBuffer->size++;
      }
      else
      {
        // current buffer is full
        // create new buffer to store the record
        // for simplicity sake
        // each buffer node only points to 1 other buffer node like a linked list
        // though each buffer node can store up to N+1 buffer nodes
        Node *newBuffer = createNewBufferNode(x, record);
        curBuffer->ptr[0] = newBuffer;
      }
    }
    // sufficient space at leafnode
    else if (curNode->size < N)
    {
      this->numKeys++;
      // make space for new key
      for (int i = curNode->size; i > insertIndex; i--)
      {
        curNode->key[i] = curNode->key[i - 1];
        curNode->ptr[i] = curNode->ptr[i - 1];
      }
      curNode->key[insertIndex] = x;
      curNode->size++;
      // create buffer for the new key
      Node *newBuffer = createNewBufferNode(x, record);
      curNode->ptr[insertIndex] = newBuffer;
      cout << "Inserted " << x << endl;
    }
    // create a new leaf node if it is already at the max
    else
    {
      cout << "Leaf node has reached, creating new leaf node\n";
      this->nodes++;
      this->numKeys++;
      // create new leaf node
      Node *newLeaf = new Node;
      int tempNode[N + 1];
      Node *buffers[N + 1];

      int insert_index = 0;
      while (x > curNode->key[insert_index] && insert_index < curNode->size)
      {
        insert_index++;
      }

      for (int i = 0; i <= N; i++)
      {
        if (i < insert_index)
        {
          tempNode[i] = curNode->key[i];
          buffers[i] = curNode->ptr[i];
        }
        else if (i == insert_index)
        {
          tempNode[i] = x;
          // create buffer for the new key
          Node *newBuffer = createNewBufferNode(x, record);
          buffers[i] = newBuffer;
        }
        else
        {
          tempNode[i] = curNode->key[i - 1];
          buffers[i] = curNode->ptr[i - 1];
        }
      }

      newLeaf->IS_LEAF = true;
      newLeaf->size = (N + 1) / 2;
      curNode->size = (N + 1) - newLeaf->size;

      Node *temp = curNode->ptr[N];
      curNode->ptr[N] = newLeaf;
      newLeaf->ptr[N] = temp;

      // fill up the new leaf node
      for (int i = 0; i < curNode->size; i++)
      {
        curNode->key[i] = tempNode[i];
        curNode->ptr[i] = buffers[i];
        if (i < newLeaf->size)
        {
          newLeaf->key[i] = tempNode[i + curNode->size];
          newLeaf->ptr[i] = buffers[i + curNode->size];
        }
      }
      // cout<<"1\n";
      // tree only has one node (root node)
      if (curNode == root)
      {
        // cout<<"2\n";
        this->levels++;
        Node *newRootNode = new Node();
        newRootNode->key[0] = newLeaf->key[0];
        newRootNode->ptr[0] = curNode;
        newRootNode->ptr[1] = newLeaf;
        newRootNode->IS_LEAF = false;
        newRootNode->size = 1;
        root = newRootNode;
        cout << "Inserted new root node successfully\n";
      }
      else
      {
        // cout<<"3\n";
        // insert new key into parent node
        insertInternal(newLeaf->key[0], parent, newLeaf);

        cout << "Inserted key into parent node successfully\n";
      }
    }
  }
}

// insertInternal operation logic
void BPlusTree::insertInternal(int x, Node *parent, Node *child)
{
  // parent is not full
  //  cout<<"a\n";
  if (parent->size < N)
  {
    // cout<<"b\n";
    int insertIndex = 0;
    while (x > parent->key[insertIndex] && insertIndex < parent->size)
    {
      insertIndex++;
    }
    // make space for new key
    for (int i = parent->size; i > insertIndex; i--)
    {
      if (i == parent->size + 1)
        parent->ptr[i] = parent->ptr[i - 1];
      else if (i == insertIndex + 1)
        parent->key[i] = parent->key[i - 1];
      else
      {
        parent->ptr[i] = parent->ptr[i - 1];
        parent->key[i] = parent->key[i - 1];
      }
    }
    parent->key[insertIndex] = x;
    parent->ptr[insertIndex + 1] = child;
    parent->size++;
    cout << "Inserted key into internal node successfully\n";
  }
  else
  {
    // cout<<"c\n";
    // create new internal node
    this->nodes++;
    Node *newInternalNode = new Node();
    int tempKey[N + 1];
    Node *tempPtr[N + 2];
    // store original in temp
    for (int i = 0; i < N; i++)
    {
      tempKey[i] = parent->key[i];
      tempPtr[i] = parent->ptr[i];
    }
    tempPtr[N] = parent->ptr[N];
    int insertIndex = 0;
    while (x > tempKey[insertIndex] && insertIndex < N)
    {
      insertIndex++;
    }

    // make space for new key
    for (int i = N + 1; i > insertIndex; i--)
    {
      tempKey[i] = tempKey[i - 1];
    }
    tempKey[insertIndex] = x;
    for (int i = N + 2; i > insertIndex; i--)
    {
      tempPtr[i] = tempPtr[i - 1];
    }
    tempPtr[insertIndex + 1] = child;
    newInternalNode->IS_LEAF = false;
    parent->size = (N + 1) / 2;
    newInternalNode->size = N - parent->size;
    // fill up the new internal node
    for (int i = 0, j = parent->size + 1; i < newInternalNode->size; i++, j++)
    {
      newInternalNode->key[i] = tempKey[j];
    }
    for (int i = 0, j = parent->size + 1; i < newInternalNode->size + 1; i++, j++)
    {
      newInternalNode->ptr[i] = tempPtr[j];
    }
    if (parent == root)
    {
      // cout<<"d\n";
      // create a new root node
      this->nodes++;
      this->levels++;
      Node *newRootNode = new Node();
      newRootNode->key[0] = parent->key[parent->size];
      newRootNode->ptr[0] = parent;
      newRootNode->ptr[1] = newInternalNode;
      newRootNode->IS_LEAF = false;
      newRootNode->size = 1;
      root = newRootNode;
      cout << "Inserted new root node successfully\n";
    }
    else
    {
      // cout<<"e\n";
      insertInternal(parent->key[parent->size], findParent(root, parent), newInternalNode);
    }
  }
}

// findParent operation logic
Node *BPlusTree::findParent(Node *curNode, Node *child)
{
  // ignore first and second level as child is at least second level
  // so the parent must at least be third level
  if (curNode->IS_LEAF || (curNode->ptr[0]->IS_LEAF))
    return NULL;
  for (int i = 0; i < curNode->size + 1; i++)
  {
    // found direct parent
    if (curNode->ptr[i] == child)
      return curNode;
    // check child of curNode to be parent of child
    else
    {
      Node *parent = findParent(curNode->ptr[i], child);
      if (parent != NULL)
        return parent;
    }
  }
  return NULL;
}

void BPlusTree::deleteKey(int x)
{
  if (root == NULL)
  {
    cout << "Empty" << endl;
  }
  else
  {
    Node *curNode = root;
    Node *parent;
    int leftPtrIndex, rightPtrIndex;
    while (curNode->IS_LEAF == false)
    {
      for (int i = 0; i < curNode->size; i++)
      {
        parent = curNode;
        
        if (x < curNode->key[i])
        {
          leftPtrIndex = i - 1;
          rightPtrIndex = i + 1;
          curNode = curNode->ptr[i];
          break;
        }
      }
      if(parent == curNode){
        leftPtrIndex = curNode->size-1;
        rightPtrIndex = curNode->size+1;
        curNode = curNode->ptr[curNode->size];
      }
    }
    int index;
    for (index = 0; index < curNode->size; index++)
    {
      //found key
      if (curNode->key[index] == x)
      {
        for (int i = index; i < curNode->size-1; i++)
        {
          curNode->key[i] = curNode->key[i + 1];
          curNode->ptr[i] = curNode->ptr[i+1];
        }
        curNode->size--;
        curNode->key[curNode->size] = 0;
        curNode->ptr[curNode->size] = NULL;

        //if leafnode has less than minimum size
        if (curNode->size < (N + 1) / 2)
        {
          //check left leafnode to take a key
          if (leftPtrIndex >= 0)
          {
            Node *leftNode = parent->ptr[leftPtrIndex];
            //leftNode has more than minimum keys
            if (leftNode->size >= ((N + 1)/2) + 1)
            {
              for (int i = curNode->size; i > 0; i--)
              {
                curNode->key[i] = curNode->key[i - 1];
                curNode->ptr[i] = curNode->ptr[i-1];
              }
              curNode->size++;
              curNode->key[0] = leftNode->key[leftNode->size - 1];
              curNode->ptr[0] = leftNode->ptr[leftNode->size -1];
              leftNode->ptr[leftNode->size-1] = NULL;
              leftNode->key[leftNode->size-1] = 0;
              leftNode->size--;
              //update the parent key to new curNode->key[0]
              parent->key[leftPtrIndex] = curNode->key[0];
              return;
            }
          }
          //check right leafnode to take a key
          if (rightPtrIndex <= parent->size)
          {
            Node *rightNode = parent->ptr[rightPtrIndex];
            //right node has more than minimum keys
            if (rightNode->size >= ((N + 1)/2) + 1)
            {
              curNode->ptr[curNode->size] = rightNode->ptr[0];
              curNode->key[curNode->size] = rightNode->key[0];
              curNode->size++;
              rightNode->size--;
              //delete the taken key
              for (int i = 0; i < rightNode->size; i++)
              {
                rightNode->key[i] = rightNode->key[i+1];
                rightNode->ptr[i] = rightNode->ptr[i+1];
              }
              rightNode->key[rightNode->size] = 0;
              rightNode->ptr[rightNode->size] = NULL;
              parent->key[rightPtrIndex - 1] = rightNode->key[0];
              return;
            }
          }
          //code reaches here if there is no excess keys from both
          //left and right leafnodes
          //merge with left leafnode
          if (leftPtrIndex >= 0)
          {
            Node *leftNode = parent->ptr[leftPtrIndex];
            for (int i = leftNode->size, j = 0; j < curNode->size; i++, j++)
            {
              leftNode->key[i] = curNode->key[j];
              leftNode->ptr[i] = curNode->ptr[j];
            }
            leftNode->size += curNode->size;
            leftNode->ptr[N] = curNode->ptr[N];
            deleteInternal(parent->key[leftPtrIndex], parent, curNode);
            delete[] curNode->key;
            delete[] curNode->ptr;
            delete curNode;
            this->nodes--;
          }
          //merge with right leafnode
          else if (rightPtrIndex <= parent->size)
          {
            Node *rightNode = parent->ptr[rightPtrIndex];
            for (int i = curNode->size, j = 0; j < rightNode->size; i++, j++)
            {
              curNode->key[i] = rightNode->key[j];
              curNode->ptr[i] = rightNode->ptr[j];
            }
            curNode->size += rightNode->size;
            curNode->ptr[N] = rightNode->ptr[N];
            // cout << "Merging two leaf nodes\n";
            deleteInternal(parent->key[rightPtrIndex - 1], parent, rightNode);
            delete[] rightNode->key;
            delete[] rightNode->ptr;
            delete rightNode;
            this->nodes--;
          }
        }
        return;
      }
    }
    cout << "Not found" << endl;
  }
}

void BPlusTree::deleteInternal(int x, Node *curNode, Node *child)
{
  if (curNode == root)
  {
    if (curNode->size == 1)
    {
      for(int i = 0; i < 2; i++){
        if(curNode->ptr[i] == child){
          //i is either 0 or 1
          root = curNode->ptr[!i];
          delete[] child->key;
          delete[] curNode->key;
          delete[] child->ptr;
          delete[] curNode->ptr;
          delete child;
          delete curNode;
          cout << "Changed root node\n";
          return;
        }
      }
    }
  }
  int index;
  for (int i = 0; i < curNode->size; i++)
  {
    if (curNode->key[i] == x){
      index = i;
      break;
    }
      
  }
  for (int i = index; i < curNode->size; i++)
  {
    curNode->key[i] = curNode->key[i + 1];
    curNode->ptr[i+1] = curNode->ptr[i+2];
  }
  curNode->size--;
  
  if (curNode->size < N/2){
    if (curNode == root)
      return;
    Node *parent = findParent(root, curNode);
    int leftPtrIndex, rightPtrIndex;
    for (int i = 0; i < parent->size + 1; i++)
    {
      if (parent->ptr[i] == curNode)
      {
        index = i;
        leftPtrIndex = index - 1;
        rightPtrIndex = index + 1;
        break;
      }
    }
    if (leftPtrIndex >= 0)
    {
      Node *leftNode = parent->ptr[leftPtrIndex];
      //if leftNode has more than minimum
      //curNode can take it
      if (leftNode->size > N/2)
      {
        curNode->ptr[curNode->size+1] = curNode->ptr[curNode->size];
        for (int i = curNode->size; i > 0; i--)
        {
          curNode->key[i] = curNode->key[i-1];
          curNode->ptr[i] = curNode->ptr[i-1];
        }
        curNode->key[0] = parent->key[leftPtrIndex];
        curNode->ptr[0] = leftNode->ptr[leftNode->size];
        parent->key[leftPtrIndex] = curNode->key[0];
        curNode->size++;
        leftNode->size--;
        return;
      }
    }
    if (rightPtrIndex <= parent->size)
    {
      Node *rightNode = parent->ptr[rightPtrIndex];
      //if rightNode has more than minimum
      //curNode can take it
      if (rightNode->size > N/2)
      {
        curNode->key[curNode->size] = parent->key[index];
        parent->key[index] = rightNode->key[0];
        for (int i = 0; i < rightNode->size - 1; i++)
        {
          rightNode->key[i] = rightNode->key[i + 1];
        }
        curNode->ptr[curNode->size + 1] = rightNode->ptr[0];
        for (int i = 0; i < rightNode->size; ++i)
        {
          rightNode->ptr[i] = rightNode->ptr[i + 1];
        }
        curNode->size++;
        rightNode->size--;
        return;
      }
    }
    if (leftPtrIndex >= 0)
    {
      Node *leftNode = parent->ptr[leftPtrIndex];
      leftNode->key[leftNode->size] = parent->key[leftPtrIndex];
      for (int i = leftNode->size + 1, j = 0; j < curNode->size; j++)
      {
        leftNode->key[i] = curNode->key[j];
      }
      for (int i = leftNode->size + 1, j = 0; j < curNode->size + 1; j++)
      {
        leftNode->ptr[i] = curNode->ptr[j];
        curNode->ptr[j] = NULL;
      }
      leftNode->size += curNode->size + 1;
      curNode->size = 0;
      deleteInternal(parent->key[leftPtrIndex], parent, curNode);
    }
    else if (rightPtrIndex <= parent->size)
    {
      Node *rightNode = parent->ptr[rightPtrIndex];
      curNode->key[curNode->size] = parent->key[rightPtrIndex - 1];
      for (int i = curNode->size + 1, j = 0; j < rightNode->size; j++)
      {
        curNode->key[i] = rightNode->key[j];
      }
      for (int i = curNode->size + 1, j = 0; j < rightNode->size + 1; j++)
      {
        curNode->ptr[i] = rightNode->ptr[j];
        rightNode->ptr[j] = NULL;
      }
      curNode->size += rightNode->size + 1;
      rightNode->size = 0;
      deleteInternal(parent->key[rightPtrIndex - 1], parent, rightNode);
    }
  }
  
}

// Search operation
void BPlusTree::search(int x)
{
  if (root == NULL)
  {
    cout << "Empty";
    return;
  }
  Node *curNode = traverseToLeafNode(x)[1];
  for (int i = 0; i < curNode->size; i++)
  {
    if (curNode->key[i] == x)
    {
      cout << "Found" << endl;
      int count = 0;
      Node *buffer = curNode->ptr[i];
      while (true)
      {
        count += buffer->size;
        if (buffer->size == N)
          buffer = buffer->ptr[0];
        else
          break;
      }
      cout << count << endl;
      return;
    }
  }
  cout << "Not found\n";
}
void BPlusTree::experiment2()
{
  cout << "Experiment 2" << endl;
  // cout << "Number of Unique Key Values: " << this->numKeys << endl;
  cout << "Parameter N: " << N << endl;
  cout << "Number of Nodes: " << this->nodes << endl;
  cout << "Number of Levels: " << this->levels << endl;
  cout << "Keys of Root Node:" << endl;
  if (root != NULL)
  {
    for (int i = 0; i < root->size; i++)
    {
      cout << "Key " << i << ": " << root->key[i] << endl;
    }
  }
}
// Search operation
void BPlusTree::experiment3(int x, Storage *storage)
{
  cout << "Experiment 3" << endl;
  if (root == NULL)
  {
    cout << "Empty" << endl;
    return;
  }
  // Get starting timepoint
  auto start = chrono::high_resolution_clock::now();
  // it is at least the number of levels
  int nodes_accessed = this->levels;
  set<int> blocks_accessed;
  Node *curNode = traverseToLeafNode(x)[1];
  for (int i = 0; i < curNode->size; i++)
  {
    if (curNode->key[i] == x)
    {
      // cout << "Found\n";
      int count = 0;
      Node *buffer = curNode->ptr[i];
      float avg_avg_rating = 0;
      while (true)
      {
        count += buffer->size;
        for (int j = 0; j < buffer->size; j++)
        {
          blocks_accessed.insert(storage->retrieve_block_id(buffer->records[j]));
          avg_avg_rating += (float)*(buffer->records[j] + storage->tconst_size) / 10;
        }
        if (buffer->size == N)
          buffer = buffer->ptr[0];
        else
          break;
      }
      avg_avg_rating /= count;
      // Get ending timepoint
      auto stop = chrono::high_resolution_clock::now();
      auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

      cout << "Index nodes accessed: " << nodes_accessed << endl;
      cout << "Number of data blocks accessed: " << blocks_accessed.size() << endl;
      cout << "Average of averageRatings: " << avg_avg_rating << endl;
      cout << "Runtime of retrieval process: " << duration.count() << "microseconds" << endl;
      storage->experiment3(x);
      return;
    }
  }
  cout << "Not found\n";
}

void BPlusTree::experiment4(int x, int y, Storage *storage)
{
  cout << "Experiment 4" << endl;
  if (root == NULL)
  {
    cout << "Empty";
    return;
  }
  int nodes_accessed = this->levels;
  set<int> blocks_accessed;
  int count = 0;
  float avg_avg_rating = 0;

  // Get starting timepoint
  auto start = chrono::high_resolution_clock::now();
  Node *curNode = traverseToLeafNode(x)[1];
  Node *temp;
  int index;
  for (int i = 0; i < curNode->size; i++)
  {
    if (curNode->key[i] >= x)
    {
      temp = curNode;
      index = i;
      break;
    }
  }
  while (temp->key[index] <= y)
  {
    // cout << "Found" << temp->key[index] << endl;
    Node *buffer = temp->ptr[index];
    while (true)
    {
      count += buffer->size;
      for (int j = 0; j < buffer->size; j++)
      {
        blocks_accessed.insert(storage->retrieve_block_id(buffer->records[j]));
        avg_avg_rating += (float)*(buffer->records[j] + storage->tconst_size) / 10;
      }
      if (buffer->size == N)
        buffer = buffer->ptr[0];
      else
        break;
    }

    index++;

    // jump to next leaf node
    if (index == temp->size)
    {
      temp = temp->ptr[N];
      index = 0;
      if (temp->key[0] <= y)
        nodes_accessed++;
    }
  }

  avg_avg_rating /= count;
  // Get ending timepoint
  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

  cout << "Index nodes accessed: " << nodes_accessed << endl;
  cout << "Number of data blocks accessed: " << blocks_accessed.size() << endl;
  cout << "Average of averageRatings: " << avg_avg_rating << endl;
  cout << "Runtime of retrieval process: " << duration.count() << "microseconds" << endl;
  storage->experiment4(x, y);
}

void BPlusTree::experiment5(int x, Storage *storage)
{
  cout << "Experiment 5" << endl;
  if (root == NULL)
  {
    cout << "Empty" << endl;
    return;
  }
  // Get starting timepoint
  auto start = chrono::high_resolution_clock::now();
  remove(x);
  
  // Get ending timepoint
  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

  cout << "Number of Nodes: " << this->nodes << endl;
  cout << "Number of Levels: " << this->levels << endl;
  cout << "Keys of Root Node:" << endl;
  if (root != NULL)
  {
    for (int i = 0; i < root->size; i++)
    {
      cout << "Key " << i << ": " << root->key[i] << endl;
    }
  }
  cout << "Runtime of retrieval process: " << duration.count() << "microseconds" << endl;
  storage->experiment5(x);
}

void BPlusTree::createTreeFromStorage(Storage *storage)
{
  // iterate through records
  // each record in storage takes up 15bytes
  // the basePtr points to the first byte in the first record
  // as such, the first 10bytes represents tconst of the first record
  // the 11th byte represents the rating
  // the 12th - 15th byte represents numVotes
  // the 16th byte represents the first byte in the second record
  // so on n so forth
  unsigned char *curPtr = storage->basePtr;
  int curRecord = 0;
  int block_no = 1;
  while (curRecord < storage->numRecords)
  {
    // vector<int> x;
    // not sure what the add_vect is for
    // x.push_back(curPtr);

    // this stored the numVotes tof the current record in key_value
    //  std::cout << storage->convertBytesToInt(curPtr+storage->tconst_size + storage->rating_size) << std::endl;
    // im assuming that x.key_value is supposed to store the numVotes of the current record
    //  if (storage->convertBytesToInt(curPtr+storage->tconst_size + storage->rating_size) == 2127){
    //    break;
    //  }
    this->insert(storage->convertBytesToInt(curPtr + storage->tconst_size + storage->rating_size), curPtr);
    // INSERTION INTO THE BPLUSTREE CAN TAKE PLACE HERE
    //  insert(x);

    // comment out this section of code to remove printing the record in terminal
    // please uncomment line 575-577 below if you comment out this section of code
    // section start//
    //  if (curRecord%storage->max_records_per_block == 0) std::cout << "Block: " << block_no << std::endl;
    //  if(*curPtr == '\0') std::cout << "Record does not exist" << std::endl;
    //  else{
    //    for(int i = 0; i< storage->tconst_size;i++){
    //      if(*curPtr != '\0') std::cout << *curPtr;
    //      curPtr++;
    //    }
    //    std::cout <<"\t";
    //    std::cout << (float)*curPtr/10 << "\t";
    //    curPtr++;
    //    std::cout << storage->convertBytesToInt(curPtr) << std::endl;
    //    curPtr+=4;
    //  }
    // section end

    // keeps track of number of records iterated
    // this is also used to track which block we are in
    curRecord++;

    // uncomment these 3 lines of code if you comment out the previous code section
    //  storage->display_record(curPtr);
    // jumps to the first byte of the next record
    curPtr += storage->record_size;

    // if curRecord%store->max_records_per_block is zero
    // it means that the current block is full
    if (curRecord % storage->max_records_per_block == 0)
    {
      // points to the first byte of the first record in the next block
      curPtr += storage->excess_block_size;
      block_no++;
      // std::cout << std::endl;
    }
  }
}

void BPlusTree::displayRecords(Storage *storage)
{
  // level order traversal
  if (!root)
  {
    return;
  }
  Node *cur = root;
  while (!cur->IS_LEAF)
  {
    cur = cur->ptr[0];
  }
  while (cur != NULL)
  {
    for (int j = 0; j < cur->size; j++)
    {
      Node *buffer = cur->ptr[j];
      while (true)
      {
        for (int i = 0; i < buffer->size; i++)
        {
          storage->display_record(buffer->records[i]);
        }
        if (buffer->size == N)
          buffer = buffer->ptr[0];
        else
          break;
      }
    }
    cur = cur->ptr[N];
  }
}

void BPlusTree::display()
{
  // level order traversal
  if (!root)
  {
    return;
  }

  queue<Node *> q;
  q.push(root);

  int currentLevelNodes = 1;
  int nextLevelNodes = 0;

  while (!q.empty())
  {
    Node *node = q.front();
    q.pop();
    currentLevelNodes--;

    for (int i = 0; i < node->size; i++)
    {
      cout << node->key[i] << " ";
    }
    cout << ",";
    if (!node->IS_LEAF)
    {
      for (int i = 0; i < node->size + 1; i++)
      {
        q.push(node->ptr[i]);
        nextLevelNodes++;
      }
    }

    if (currentLevelNodes == 0)
    {
      cout << endl; // print a new line for every layer
      currentLevelNodes = nextLevelNodes;
      nextLevelNodes = 0;
    }
  }
}