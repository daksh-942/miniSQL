#pragma once
#include <fstream>
#include <string>

static const uint32_t PAGE_SIZE = 4096;

class Pager 
{
private:
    std::fstream file;
    std::string filename;
    uint32_t page_count;

public:
    Pager(const std::string& fname);
    ~Pager();

    void read_page(uint32_t page_id, char* buffer);
    void write_page(uint32_t page_id, const char* buffer);

    uint32_t allocate_page();
    uint32_t get_page_count() const;
};
