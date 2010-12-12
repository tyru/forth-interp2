/* 
 * forth.cpp
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2010-12-12.
 *
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include <map>
#include <boost/lexical_cast.hpp>


typedef int ForthValue;
std::stack<ForthValue> forth_stack;

typedef void (*WordFn)();
std::map<std::string, WordFn> forth_words;



void
forth_run(const std::string& code)
{
    std::istringstream ss(code);
    std::string token;
    while (ss) {
        ss >> token;
        if (forth_words.find(token) != forth_words.end()) {
            forth_words[token]();
        }
        else {
            try {
                forth_stack.push(boost::lexical_cast<ForthValue>(token));
            }
            catch (boost::bad_lexical_cast& e) {
                std::cerr << "can't convert '" << token << "' to integer." << std::endl;
            }
        }
    }
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
    forth_run(buf);
}
