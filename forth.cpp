/* 
 * forth.cpp
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2010-12-12.
 *
 */


#include <iostream>
#include <fstream>
#include <string>


void
run(const std::string& code)
{
    std::cout << code << std::endl;
}

int
main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: ./forth {file}" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << argv[1] << "." << std::endl;
        return 1;
    }

    std::string buf(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    run(buf);
}
