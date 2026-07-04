#pragma once
#include <cstdint> 
#include <fstream>
#include <string>

static const uint32_t PAGE_SIZE = 4096;

class Pager 
{
private:
    std::fstream file; // File stream for reading and writing pages
    std::string filename; // Name of the file being paged
    uint32_t page_count;

public:
    Pager(const std::string& fname); //Runs when database starts.
    ~Pager();

    void write_page(uint32_t page_id, const char* buffer);
    void read_page(uint32_t page_id, char* buffer);

    uint32_t allocate_page();
    uint32_t get_page_count() const;
};
