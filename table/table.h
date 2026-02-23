#pragma once
#include "../storage/pager.h"
#include "../btree/diskBtree.h"
#include "../page/slotted_page.h"
#include "row.h"

class Table{
    private:
    Pager* pager;  //pointer since pager is same for all the tables (singleton we dont create pager again and again)
    DiskBtree tree; //here direct object is created because every table has its own B+ TREE.

    uint32_t page_id;  //every row will be stored in this same page only for simplicity 

    public:

    Table(Pager* pager);

    void insert(const Row& row);
    bool get(uint32_t key,Row& out);

};