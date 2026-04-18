#include "cli.h"
#include <iostream>
#include <sstream>

CLI::CLI(const std::string& db_file)
    : pager(db_file), table(&pager) {}

void CLI::print_help() {

    std::cout << "\nAvailable commands:\n";
    std::cout << "---------------------------------\n";
    std::cout << "insert <id> <name>   : Insert row\n";
    std::cout << "get <id>             : Get row\n";
    std::cout << "delete <id>          : Delete row\n";
    std::cout << "update <id> <name>   : Update row\n";
    std::cout << "scan <l> <r>         : Range scan\n";
    std::cout << "print                : Print table\n";
    std::cout << "count                : Count rows\n";
    std::cout << "exists <id>          : Check if exists\n";
    std::cout << "clear                : Reset table\n";
    std::cout << "help                 : Show commands\n";
    std::cout << "exit                 : Exit program\n";
    std::cout << "---------------------------------\n\n";
}

void CLI::run() {

    std::string line;

    while (true) {

        std::cout << "db> ";
        std::getline(std::cin, line);

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "exit") {
            std::cout << "Bye!\n";
            break;
        }
        else if (cmd == "insert") {

            int id;

            if (!(ss >> id)) {
                std::cout << "Invalid input\n";
                continue;
            }

            // read full remaining string (supports spaces)
            std::string name;
            std::getline(ss, name);

            // remove leading space
            if (!name.empty() && name[0] == ' ')
                name.erase(0, 1);

            if (name.empty()) {
                std::cout << "Name required\n";
                continue;
            }

            Row r1;
            if (table.get(id, r1)) {
                std::cout << "Key already exists\n";
            } else {
                Row r;
                r.key = id;
                snprintf(r.record, sizeof(r.record), "%s", name.c_str());
                table.insert(r);
                std::cout << "Inserted\n";
            }
        }
        else if (cmd == "get") {

            int id;
            if (!(ss >> id)) {
                std::cout << "Invalid input\n";
                continue;
            }

            Row r;
            if (table.get(id, r))
                std::cout << r.key << " -> " << r.record << "\n";
            else
                std::cout << "Not found\n";
        }

        else if (cmd == "delete") {

            int id;
            if (!(ss >> id)) {
                std::cout << "Invalid input\n";
                continue;
            }

            if (table.remove(id))
                std::cout << "Deleted\n";
            else
                std::cout << "Not found\n";
        }

        else if (cmd == "update") {

            int id;
            std::string name;

            if (!(ss >> id >> name)) {
                std::cout << "Invalid input\n";
                continue;
            }

            Row r;
            r.key = id;
            snprintf(r.record, sizeof(r.record), "%s", name.c_str());

            if (table.update(r))
                std::cout << "Updated\n";
            else
                std::cout << "Not found\n";
        }

        else if (cmd == "scan") {

            int l, r;
            if (!(ss >> l >> r)) {
                std::cout << "Invalid input\n";
                continue;
            }

            table.scan(l, r);
        }

        else if (cmd == "print") {
            table.print_all();
        }

        else if (cmd == "count") {
            std::cout << "Total rows: " << table.count_rows() << "\n";
        }

        else if (cmd == "clear") {
            table.clear();
            std::cout << "Table cleared\n";
        }

        else if (cmd == "exists") {

            int id;
            if (!(ss >> id)) {
                std::cout << "Invalid input\n";
                continue;
            }

            Row r;
            if (table.get(id, r))
                std::cout << "Yes\n";
            else
                std::cout << "No\n";
        }

        else if (cmd == "help") {
            print_help();
        }

        else {
            std::cout << "Unknown command\n";
        }
    }
}