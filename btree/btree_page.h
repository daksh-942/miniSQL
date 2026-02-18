#pragma once
#include "../storage/pager.cpp"
#include <cstint>
#include <cstring>

class BtreePage {
    private : 
    char* data;

    static const int header_size = 11;

    public :
    static const int ORDER = 4;
    static const int MAX_KEYS = ORDER-1;

    BtreePage (char* buffer);
    
    bool is_leaf();
    void set_leaf(bool leaf);

    int* get_keys();

    int* get_children();

    int get_next();
    void set_next(int page_id);

    int parent();
    void set_parent(int page_id);

    int get_key_count();
    void set_key_count(int count);
    
    std :: pair<uint32_t,uint16_t> *values();


};