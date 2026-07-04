#pragma once
#include "./btree_page.h"

class DiskBtree{
    private:
        Pager *pager;
        uint32_t root_page;

        void insert_recursive(int key, std ::pair<uint32_t, uint16_t> value, uint32_t page_id);

        void split_leaf(uint32_t page_id,int key,std::pair<uint32_t,uint16_t> value);

        void split_internal(uint32_t page_id);

        void insert_into_parent(uint32_t left_page,int key, uint32_t right_page);
        
    public:
        DiskBtree(Pager *pager);

        void insert(uint32_t key, std ::pair<uint32_t, uint16_t> value);

        std ::pair<uint32_t, uint16_t> search(int key);

        bool remove(int key);  //we are using tombstone deletion so we will not be merging the nodes and we will not be rebalancing the tree after deletion
                                //also called lazy deletion as we are not rebalancing the tree after deletion and we are not merging the nodes after deletion
};