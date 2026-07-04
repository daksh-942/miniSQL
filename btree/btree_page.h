#pragma once
#include "../storage/pager.h"
#include <cstdint>
#include <cstring>

// Byte Offset

// 0        is_leaf (1 byte)

// 1-2      key_count (2 bytes)

// 3-6      parent_page (4 bytes)

// 7-10     next_leaf_page (4 bytes)

// ---------------------------------

// 11...    keys[]

// ---------------------------------

//         children[] (internal node)
//         OR
//         values[]   (leaf node)

// +--------------------------------------------------+
// | Header (11 bytes)                                |
// +--------------------------------------------------+
// | Keys                                              |
// +--------------------------------------------------+
// | Children OR Values                                |
// +--------------------------------------------------+



class BtreePage //this class itself is a node , whiile calling any of it function we need not to sent curr node anytime
{
    private : 
    char* data;

    static const int header_size = 11;

    public :
    static const int ORDER = 4;
    static const int MAX_KEYS = ORDER-1;

    BtreePage (char* buffer);
    
    bool is_leaf();
    void set_leaf(bool leaf);

    uint16_t get_key_count();
    void set_key_count(uint16_t count);
    
    int* get_keys();

    uint32_t* get_children();

    uint32_t get_next();
    void set_next(uint32_t page_id);

    uint32_t parent();
    void set_parent(uint32_t page_id);
    std :: pair<uint32_t,uint16_t> *values();
};