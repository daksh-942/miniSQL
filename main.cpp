

/************************WE ARE TESTING PAGER HERE*********************************** */
// #include "storage/pager.h"
// #include <iostream>
// #include <cstring>
// #include <string>

// int main()
// {
//     Pager pager("1st_db");
//     uint32_t a1=pager.allocate_page();
//     char buffer[PAGE_SIZE]={0};
//     std::string s="1st day";
//     for (int i=0;i<s.size();i++) buffer[i]=s[i];
//     // pager.write_page(a1,buffer);
//     char rd[PAGE_SIZE];
//     pager.read_page(0,rd);
//     std::cout<<rd<<std::endl;
// }



/************************WE ARE TESTING SLOTTED PAGE HERE*********************************** */

#include "storage/pager.h"
#include "page/slotted_page.h"
#include <iostream>
#include <cstring>

int main() {

    Pager pager("test.db");

    uint32_t page_id;

    if (pager.get_page_count() == 0) {
        page_id = pager.allocate_page();

        char init_buffer[PAGE_SIZE];
        pager.read_page(page_id, init_buffer);

        Slotted_Page init_page(init_buffer);
        init_page.initialize();

        pager.write_page(page_id, init_buffer);
    }

    page_id = 0;

    char buffer[PAGE_SIZE];
    pager.read_page(page_id, buffer);

    Slotted_Page page(buffer);

    const char* msg1 = "Hello World";
    const char* msg2 = "Database Internals";

    int s1 = page.insert(msg1, strlen(msg1) + 1);
    int s2 = page.insert(msg2, strlen(msg2) + 1);

    pager.write_page(page_id, buffer);

    char out[100];

    page.read(s1, out);
    std::cout << "Slot 1: " << out << std::endl;

    page.read(s2, out);
    std::cout << "Slot 2: " << out << std::endl;

    return 0;
}