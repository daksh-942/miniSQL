#include "btree.h"
#include <iostream>
Node :: Node(bool leaf){
    is_leaf=leaf;
    next=nullptr;
}
BTree :: BTree(int order){
    ORDER=order;
    root=new Node(true);
}

std :: pair<int,int>* BTree :: search(int key){
    Node* current=root;
    while(!current->is_leaf){

        int i=0;
        while(i<current->keys.size()&&key>=current->keys[i]){
            i++;
        }
        current=current->children[i];
    }

    for(int i=0;i<current->keys.size();i++){

        if(current->keys[i]==key){
            return &current->values[i];
        }
    }
    return nullptr;
}

void BTree :: insert(int key,std :: pair<int,int> value){
    if(root->keys.size()==ORDER-1){
        Node* new_root=new Node(false);
        new_root->children.push_back(root);

        split_child(new_root,0);
        root=new_root;
    }
    insert_non_full(root,key,value);
}

void BTree :: insert_non_full(Node* node,int key,std :: pair<int,int> value){
    if(node->is_leaf){

        int i=node->keys.size()-1;
        node->keys.push_back(0);
        node->values.push_back({0,0});

        while(i>=0 && (key < node->keys[i])){
            node->keys[i+1]=node->keys[i];
            node->values[i+1]=node->values[i];
            i--;
        }
        node->keys[i]=key;
        node->values[i]=value;
    }
    else{
        int i=0;
        while((i<node->keys.size()) && (key>=node->keys[i])){
            i++;
        }
        Node* child=node->children[i];
        if(child->keys.size()==ORDER-1){
            split_child(node,i);
            if(key>=node->keys[i])
                child=node->children[i+1];
        }
        insert_non_full(child,key,value);
    }
}

void BTree :: split_child(Node* parent,int index){
    Node* left=parent->children[index];
    Node* right=new Node(left->is_leaf);

    int mid=ORDER/2;

    if(left->is_leaf){

        for(int i=mid;i<left->keys.size();i++){
            right->keys.push_back(left->keys[i]);
            right->values.push_back(left->values[i]);
        }
        left->keys.resize(mid);
        left->values.resize(mid);

        right->next=left->next;
        left->next=right;

        parent->keys.insert(parent->keys.begin()+index,right->keys[0]);
    }
    else{
        int promote_key=left->keys[mid];

        for(int i=mid+1;i<left->keys.size();i++){
            right->keys.push_back(left->keys[i]);
        }
        for(int i=mid+1;i<left->children.size();i++){
            right->children.push_back(left->children[i]);
        }

        left->keys.resize(mid);
        left->children.resize(mid+1);

        parent->keys.insert(parent->keys.begin()+index,promote_key);
    }
    parent->children.insert(parent->children.begin()+index+1,right);
}

void BTree::print_tree() {
    std::cout << "Tree structure:\n";

    std::vector<Node*> level;
    level.push_back(root);

    while (!level.empty()) {

        std::vector<Node*> next_level;

        for (auto node : level) {

            std::cout << "[ ";
            for (int k : node->keys)
                std::cout << k << " ";
            std::cout << "] ";

            if (!node->is_leaf) {
                for (auto child : node->children)
                    next_level.push_back(child);
            }
        }

        std::cout << "\n";
        level = next_level;
    }
}

void BTree::print_leaves() {

    Node* current = root;

    while (!current->is_leaf)
        current = current->children[0];

    std::cout << "Leaves: ";

    while (current) {
        for (int k : current->keys)
            std::cout << k << " ";
        current = current->next;
    }

    std::cout << "\n";
}