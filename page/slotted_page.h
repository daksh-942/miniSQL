#pragma once
#include<cstdint>
#include<cstring>

struct PageHeader {
    uint16_t free_space_offset;
    uint16_t slot_count;
};

#pragma pack(push,1)

struct Slot{
    uint8_t is_deleted;
    uint16_t length;
    uint16_t offset;
};

#pragma pack(pop)

class Slotted_Page{
    private:
    char* data;

    void set_header(const PageHeader& header);
    PageHeader get_header();

    void set_slot(uint16_t slot_id, Slot& slot);
    Slot get_slot(const uint16_t slot_id);


    public:

    Slotted_Page(char *page_data);

    void initialize();

    int insert(const char* record ,uint16_t length);
    void read(uint16_t slot_id,char* out_buffer);
    void remove(uint16_t slot_id);
};

