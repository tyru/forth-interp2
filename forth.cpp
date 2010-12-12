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


struct forth_stack_underflow
{
    forth_stack_underflow(const std::string& msg)
        : msg_(msg) {}

    std::string what() throw() { return msg_; }

private:
    std::string msg_;
};


void
word_plus()
{
    if (forth_stack.empty()) throw forth_stack_underflow("+");
    ForthValue x = forth_stack.top(); forth_stack.pop();
    if (forth_stack.empty()) throw forth_stack_underflow("+");
    ForthValue y = forth_stack.top(); forth_stack.pop();

    forth_stack.push(x + y);
}

void
word_print()
{
    if (forth_stack.empty()) throw forth_stack_underflow(".");
    ForthValue x = forth_stack.top(); forth_stack.pop();

    std::cout << x;
}

void
forth_init_words()
{
    forth_words.clear();

    forth_words["+"] = word_plus;
    forth_words["."] = word_print;
}

void
forth_run(const std::string& code)
{
    forth_init_words();
    std::istringstream ss(code);
    std::string token;
    while (1) {
        ss >> token;
        if (!ss) break;
        if (forth_words.find(token) != forth_words.end()) {
            try {
                forth_words[token]();
            }
            catch (forth_stack_underflow& e) {
                std::cerr << e.what() << ": no more items on the stack." << std::endl;
            }
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
