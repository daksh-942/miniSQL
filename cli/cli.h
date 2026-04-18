#pragma once
#include "../storage/pager.h"
#include "../table/table.h"

class CLI {
private:
    Pager pager;
    Table table;

public:
    CLI(const std::string& db_file);

    void run();
    void print_help();
};