#include <iostream>
#include "slotted_page.h"
#include "../storage/pager.h"

Slotted_Page :: Slotted_Page (char* page_data){
    data=page_data;
}

void Slotted_Page :: set_header(const PageHeader& header){
    memcpy(data,&header,sizeof(PageHeader));
}

PageHeader Slotted_Page :: get_header(){
    PageHeader header;
    memcpy(&header,data,sizeof(PageHeader));
    return header;
}

void Slotted_Page :: set_slot(uint16_t slot_id, Slot& slot){
    memcpy(data+sizeof(PageHeader)+slot_id*sizeof(Slot),&slot,sizeof(Slot));
}

Slot Slotted_Page :: get_slot(const uint16_t slot_id){
    Slot temp;
    memcpy(&temp,data+sizeof(PageHeader)+slot_id*sizeof(Slot),sizeof(Slot));
    return temp;
}


void Slotted_Page :: initialize(){
    PageHeader header;
    header.slot_count=0;
    header.free_space_offset=PAGE_SIZE;
    set_header(header);
}




uint16_t Slotted_Page :: insert(const char* record ,uint16_t length){
    PageHeader temp=get_header();
    int count=temp.slot_count;
    int left=sizeof(PageHeader) + count*sizeof(Slot);
    int right=temp.free_space_offset;
    if((right-left) < (length + sizeof(Slot))){
        std :: cerr<<"Space Not Available";
        return -1;
    }

    int off=right-length;
    memcpy(data+off,record,length);

    Slot sl;
    sl.is_deleted=0;
    sl.length=length;
    sl.offset=off;

    set_slot(count,sl);

    temp.slot_count=count+1;
    temp.free_space_offset-=length;
    set_header(temp);
    return count;
}


void Slotted_Page :: read(uint16_t slot_id,char* out_buffer){
      PageHeader temp=get_header();
      int count=temp.slot_count;

      if(slot_id >= count){
        std :: cerr<<"Accessing out of bound";
        exit(1);
      }

      Slot sl=get_slot(slot_id);
      if(sl.is_deleted){
        std :: cerr<<"File not Found";
        exit(1);
      }

      memcpy(out_buffer,data+sl.offset,sl.length);
}

void Slotted_Page :: remove(uint16_t slot_id){
      PageHeader temp=get_header();
      int count=temp.slot_count;

      if(slot_id >= count){
        std :: cerr<<"Accessing out of bound";
        exit(1);
      }

      Slot sl=get_slot(slot_id);
      if(sl.is_deleted){
        std :: cerr<<"Already deleted";
        exit(1);
      }

      sl.is_deleted=1;
      set_slot(slot_id,sl);
}



