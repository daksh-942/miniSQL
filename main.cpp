#include "storage/pager.h"
#include <iostream>
#include <cstring>
#include <string>

int main()
{
    Pager pager("1st_db");
    uint32_t a1=pager.allocate_page();
    char buffer[PAGE_SIZE]={0};
    std::string s="1st day";
    for (int i=0;i<s.size();i++) buffer[i]=s[i];
    // pager.write_page(a1,buffer);
    char rd[PAGE_SIZE];
    pager.read_page(0,rd);
    std::cout<<rd<<std::endl;
}