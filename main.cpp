

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

// #include "storage/pager.h"
// #include "page/slotted_page.h"
// #include <iostream>
// #include <cstring>

// int main() {

//     Pager pager("test.db");

//     uint32_t page_id;

//     if (pager.get_page_count() == 0) {
//         page_id = pager.allocate_page();

//         char init_buffer[PAGE_SIZE];
//         pager.read_page(page_id, init_buffer);

//         Slotted_Page init_page(init_buffer);
//         init_page.initialize();

//         pager.write_page(page_id, init_buffer);
//     }

//     page_id = 0;

//     char buffer[PAGE_SIZE];
//     pager.read_page(page_id, buffer);

//     Slotted_Page page(buffer);

//     const char* msg1 = "Hello World";
//     const char* msg2 = "Database Internals";

//     int s1 = page.insert(msg1, strlen(msg1) + 1);
//     int s2 = page.insert(msg2, strlen(msg2) + 1);

//     pager.write_page(page_id, buffer);

//     char out[100];

//     page.read(s1, out);
//     std::cout << "Slot 1: " << out << std::endl;

//     page.read(s2, out);
//     std::cout << "Slot 2: " << out << std::endl;

//     return 0;
// }
/************************WE ARE TESTING B+TREE HERE*********************************** */
// #include "btree/btree.h"
// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <random>

// int main() {

//     BTree tree(4);   // small order to force many splits

//     // 🔥 Test 1: Insert shuffled numbers
//     std::vector<int> nums;
//     for (int i = 1; i <= 50; i++)
//         nums.push_back(i);

//     std::random_device rd;
//     std::mt19937 g(rd());
//     std::shuffle(nums.begin(), nums.end(), g);

//     std::cout << "Inserting values...\n";

//     for (int x : nums)
//         tree.insert(x, {x, x});

//     std::cout << "\n=== TREE STRUCTURE ===\n";
//     tree.print_tree();

//     std::cout << "\n=== LEAF ORDER ===\n";
//     tree.print_leaves();

//     // 🔥 Test 2: Verify all keys exist
//     std::cout << "\n=== SEARCH TEST ===\n";
//     bool ok = true;

//     for (int i = 1; i <= 50; i++) {
//         auto res = tree.search(i);
//         if (!res) {
//             std::cout << "Missing key: " << i << "\n";
//             ok = false;
//         }
//     }

//     if (ok)
//         std::cout << "All keys found successfully.\n";

//     // 🔥 Test 3: Check missing keys
//     std::cout << "\n=== NEGATIVE TEST ===\n";
//     for (int i = 100; i <= 105; i++) {
//         auto res = tree.search(i);
//         if (res)
//             std::cout << "Error: found nonexistent key " << i << "\n";
//     }

//     // 🔥 Test 4: Stress Insert More
//     std::cout << "\n=== EXTRA INSERTS ===\n";
//     for (int i = 51; i <= 80; i++)
//         tree.insert(i, {i, i});

//     tree.print_tree();
//     tree.print_leaves();

//     std::cout << "\nDone.\n";

//     return 0;
// }



/*************************************We are testing DiskBtree**************************************************************/
// #include <iostream>
// #include "storage/pager.h"
// #include "./btree/diskBtree.h"

// int main() {

//     // Open database file
//     Pager pager("test.db");

//     // Create or load tree
//     DiskBtree tree(&pager);

//     std::cout << "Inserting keys...\n";

//     // Insert some keys
//     for (int i = 1; i <= 20; i++) {
//         tree.insert(i, {i * 10, (uint16_t)i});
//     }

//     std::cout << "Insertion done.\n\n";

//     // Search test
//     std::cout << "Searching keys...\n";

//     for (int i = 1; i <= 20; i++) {
//         auto result = tree.search(i);

//         std::cout << "Key " << i
//                   << " -> (" << result.first
//                   << ", " << result.second << ")\n";
//     }

//     // Search for missing key
//     int missing = 999;
//     auto res = tree.search(missing);

//     std::cout << "\nSearch missing key " << missing
//               << " -> (" << res.first
//               << ", " << res.second << ")\n";

//     return 0;
// }
/*************************************We are testing Table Layer**************************************************************/
// #include "storage/pager.h"
// #include "table/table.h"
// #include <iostream>
// #include <cstdio>

// int main() {

//     // 🔥 Start fresh (delete old DB file)
//     std::remove("table.db");

//     std::cout << "=== INSERT TEST ===\n";

//     {
//         Pager pager("table.db");
//         Table table(&pager);

//         // Insert a few rows
//         Row r1 = {1, "Alice"};
//         Row r2 = {2, "Bob"};
//         Row r3 = {3, "Charlie"};

//         table.insert(r1);
//         table.insert(r2);
//         table.insert(r3);

//         std::cout << "Inserted 3 rows\n";
//     }

//     // 🔁 Simulate program restart
//     std::cout << "\n=== READ TEST (after restart) ===\n";

//     {
//         Pager pager("table.db");
//         Table table(&pager);

//         for (int id = 1; id <= 3; id++) {
//             Row r;

//             if (table.get(id, r))
//                 std::cout << r.key << " -> " << r.record << "\n";
//             else
//                 std::cout << "Not found: " << id << "\n";
//         }

//         // Test missing key
//         Row r;
//         if (!table.get(99, r))
//             std::cout << "Key 99 not found (correct)\n";
//     }

//     return 0;
// }

/*************************************We are testing Table Layer**************************************************************/
#include "storage/pager.h"
#include "table/table.h"
#include <iostream>
#include <cstdio>

int main() {

    // 🔥 Start fresh
    std::remove("table.db");

    std::cout << "=== INSERT TEST ===\n";

    {
        Pager pager("table.db");
        Table table(&pager);

        // Insert enough rows to force multiple pages
        for (int i = 1; i <= 200; i++) {
            Row r;
            r.key = i;
            sprintf(r.record, "User_%d", i);
            table.insert(r);
        }

        std::cout << "Inserted 30 rows\n";
    }

    // 🔁 Restart simulation
    std::cout << "\n=== READ TEST (after restart) ===\n";

    {
        Pager pager("table.db");
        Table table(&pager);

        // Test single lookups
        Row r;

        if (table.get(5, r))
            std::cout << "Found: " << r.key << " -> " << r.record << "\n";

        if (table.get(65, r))
            std::cout << "Found: " << r.key << " -> " << r.record << "\n";

        // Test missing key
        if (!table.get(500, r))
            std::cout << "Key 500 not found (correct)\n";

        // Test range scan
        std::cout << "\n=== RANGE SCAN 10–15 ===\n";
        table.scan(1, 200);
    }

    std::cout << "\n=== TEST COMPLETE ===\n";

    return 0;
}