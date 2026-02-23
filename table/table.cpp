#include "table.h"
#include <iostream>
Table :: Table(Pager* pager):pager(pager),tree(pager){

    if(pager->get_page_count()<3){

        page_id=pager->allocate_page();
        char BUFFER[PAGE_SIZE]={0};
        Slotted_Page sl_page(BUFFER);
        sl_page.initialize();

        pager->write_page(page_id,BUFFER);
    }
    else{
        page_id=2;
    }
}
void Table :: insert(const Row& row){

    char BUFFER[PAGE_SIZE];
    pager->read_page(page_id,BUFFER);

    Slotted_Page sl_page(BUFFER);
    uint16_t slot_id=sl_page.insert((const char*)& row,sizeof(Row));

    if(slot_id<0){
        std :: cerr<<"Data page full\n";
        return;
    }

    pager->write_page(page_id,BUFFER);

    tree.insert(row.key,{page_id,slot_id});

}
bool Table :: get(uint32_t key,Row& out){

    auto ptr=tree.search(key);

    if(ptr.first==0){
        return false;
    }

    uint32_t p_id=ptr.first;
    uint16_t s_id=ptr.second;

    char BUFFER[PAGE_SIZE];
    pager->read_page(p_id,BUFFER);

    Slotted_Page sl_page(BUFFER);
    sl_page.read(s_id,(char*)& out);

    return true;
}