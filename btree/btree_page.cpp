#include "btree_page.h"

BtreePage::BtreePage(char* buffer) {
    data = buffer;
}

bool BtreePage::is_leaf() {
    return data[0];
}

void BtreePage::set_leaf(bool leaf) {
    data[0] = leaf;
}


uint16_t BtreePage::get_key_count() {
    uint16_t v;
    memcpy(&v, data + 1, 2);
    return v;
}

void BtreePage::set_key_count(uint16_t count) {
    memcpy(data + 1, &count, 2);
}


uint32_t BtreePage::parent() {
    uint32_t v;
    memcpy(&v, data + 3, 4);
    return v;
}

void BtreePage::set_parent(uint32_t page_id) {
    memcpy(data + 3, &page_id, 4);
}

uint32_t BtreePage::get_next() {
    uint32_t v;
    memcpy(&v, data + 7, 4);
    return v;
}


void BtreePage::set_next(uint32_t page_id) {
    memcpy(data + 7, &page_id, 4);
}


int* BtreePage::get_keys() {
    return (int*)(data + header_size);
}

uint32_t* BtreePage::get_children() {
    return (uint32_t*)
        (data + header_size + (MAX_KEYS+2) * sizeof(int));  // we are allocating space for MAX_KEYS+2 keys in the page so that we can insert a new key and then split the page if needed
}



std::pair<uint32_t,uint16_t>* BtreePage::values() {
    return (std::pair<uint32_t,uint16_t>*)
        (data + header_size + (MAX_KEYS+2) * sizeof(int));
}
