#pragma once
#include <vector>
#include "../storage/pager.h"
#include "../btree/diskBtree.h"
#include "../page/slotted_page.h"
#include "row.h"

class Table{
    private:
    Pager* pager;  //pointer since pager is same for all the tables (singleton we dont create pager again and again)
    DiskBtree tree; //here direct object is created because every table has its own B+ TREE.

    uint32_t meta_page;
    std :: vector<uint32_t> data_pages; 

    void load_metadata();
    void save_metadata();


    public:

    Table(Pager* pager);

    void insert(const Row& row);
    bool get(uint32_t key,Row& out);

    void scan(uint32_t key_start, uint32_t key_end);

};