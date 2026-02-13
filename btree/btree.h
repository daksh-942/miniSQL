#pragma once
#include <vector>
#include <utility>
// #include <algorithm>

struct Node{

    bool is_leaf;

    //internal
    std :: vector<int> keys;
    std :: vector<Node*> children;

    //leaf
    std :: vector<std :: pair<int,int>> values;  //{page_id,slot_id}

    Node* next;

    Node(bool leaf);

};


class BTree{

    private:
        int ORDER;
        Node* root;

        void insert_non_full(Node* node,int key,std :: pair<int,int> value);
        void split_child(Node* parent,int index);

    public:
        BTree(int order=4);

        void insert(int key, std :: pair<int,int> value);

        std :: pair<int,int>* search(int key);

        //for debugging
        void print_tree();
        void print_leaves();

};

