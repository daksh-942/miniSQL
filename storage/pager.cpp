#include <iostream>
#include "pager.h"

Pager::Pager(const std::string& fname): filename(fname)
{
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.open(filename,std::ios::out | std::ios:: binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
    file.seekg(0,std::ios::end);
    std::streampos size=file.tellg();
    if (size%PAGE_SIZE!=0)
    {
        std::cerr<<"file corrupted";
        exit(1);
    }
    page_count=size/PAGE_SIZE;
}

void Pager::read_page(uint32_t page_id, char* buffer)
{
    if (page_id>=page_count)
    {
        std::cerr<<"wrong page asked";
        exit(1);
    }
    file.seekg(page_id*PAGE_SIZE,std::ios::beg);
    file.read(buffer,PAGE_SIZE);
    if (file.gcount()<PAGE_SIZE)
    {
        std::cerr<<"wrong formatting of the page";
        exit(1);
    }
}

void Pager::write_page(uint32_t page_id, const char* buffer)
{
    if (page_id>=page_count)
    {
        std::cerr<<"wrong page asked";
        exit(1);
    }
    file.seekp(page_id*PAGE_SIZE,std::ios::beg);
    file.write(buffer,PAGE_SIZE);
    file.flush();
}

uint32_t Pager::allocate_page()
{
    file.seekp(0,std::ios::end);
    const char buffer[PAGE_SIZE]={0};
    file.write(buffer,PAGE_SIZE);
    page_count++;
    file.flush();
    return page_count-1;
}

Pager::~Pager()
{
    if (file.is_open()) 
        file.close();
}

uint32_t Pager::get_page_count() const
{
    return  page_count;
}