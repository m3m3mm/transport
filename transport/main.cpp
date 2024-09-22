#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    try {
        TransportCatalogue catalogue;

        int base_request_count;
        std::cin >> base_request_count;
        std::cin.ignore();

        {
            InputReader reader;
            for (int i = 0; i < base_request_count; ++i) {
                std::string line;
                std::getline(std::cin, line);
                reader.ParseLine(line);
            }
            reader.ApplyCommands(catalogue);
        }

        int stat_request_count;
        std::cin >> stat_request_count;
        std::cin.ignore();
        for (int i = 0; i < stat_request_count; ++i) {
            std::string line;
            std::getline(std::cin, line);
            ParseAndPrintStat(catalogue, line, std::cout);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
