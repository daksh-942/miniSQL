#include "table.h"
#include <iostream>

void Table :: load_metadata(){
    char BUFFER[PAGE_SIZE]={0};
    pager->read_page(meta_page,BUFFER);

    uint32_t count;
    memcpy(&count,BUFFER,4);

    data_pages.clear(); //out of safety 


    for(uint32_t i=0;i<count;i++){
        uint32_t temp_page;
        memcpy(&temp_page,BUFFER+(i+1)*4,4);
        data_pages.push_back(temp_page);
    }
}

void Table :: save_metadata(){
    char BUFFER[PAGE_SIZE]={0};
    uint32_t count=data_pages.size();
    memcpy(BUFFER,&count,4);

    for(uint32_t i=0;i<count;i++){
        memcpy(BUFFER+(i+1)*4,&data_pages[i],4);
    }

    pager->write_page(meta_page,BUFFER);

}


Table :: Table(Pager* pager):pager(pager),tree(pager){

    if(pager->get_page_count()<3){

        uint32_t page_id = pager->allocate_page();
        char BUFFER[PAGE_SIZE]={0};
        Slotted_Page sl_page(BUFFER);
        sl_page.initialize();
        pager->write_page(page_id,BUFFER);
        data_pages.push_back(page_id);


        meta_page=pager->allocate_page();
        save_metadata();
    }
    else{
        meta_page=3;
        load_metadata();
    }
}
void Table :: insert(const Row& row){

    char BUFFER[PAGE_SIZE]={0};

    int count=data_pages.size();
    int slot_id=-1;
    uint32_t page_id;

    for(int i=0;i<count;i++){

        page_id=data_pages[i];

        pager->read_page(page_id,BUFFER);

        Slotted_Page sl_page(BUFFER);
        slot_id=sl_page.insert((const char*)& row,sizeof(Row));

        if(slot_id<0){
            continue;
        }
        break;
    }

    if(slot_id<0){

        page_id=pager->allocate_page();
        pager->read_page(page_id,BUFFER);

        data_pages.push_back(page_id);

        save_metadata();

        Slotted_Page sl_page(BUFFER);
        sl_page.initialize();

        slot_id=sl_page.insert((const char*)& row,sizeof(Row));

        if(slot_id<0){
            return ;
        }
    }
    
    pager->write_page(page_id,BUFFER);
    tree.insert(row.key,{page_id,(uint16_t)slot_id});

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

void Table :: scan(uint32_t key_start, uint32_t key_end){
    //brute method directly loop down from start to end but and call get for each key but it will take nLogn
    for(uint32_t i=key_start;i<=key_end;i++){
        Row r;
        if(get(i,r)){
            std::cout << r.key << " -> " << r.record << "\n";
        }
    }
}

bool Table :: remove(int key){
    auto ptr=tree.search(key);
    if (ptr.first==0) return false;

    uint32_t p_id=ptr.first;
    uint16_t s_id=ptr.second;

    char BUFFER[PAGE_SIZE];
    pager->read_page(p_id,BUFFER);

    Slotted_Page sl(BUFFER);
    sl.remove(s_id);
    pager->write_page(p_id,BUFFER);

    tree.remove(key);
    return true;
}

bool Table :: update(const Row& row){
    if (!remove(row.key)){
        return false;
    }
    insert(row);
    return true;
}
