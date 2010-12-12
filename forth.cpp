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


namespace forth {


typedef int ForthValue;
typedef std::stack<ForthValue> ForthStack;
typedef void (*WordFn)(ForthStack&);
typedef std::map<std::string, WordFn> ForthWordMap;



struct stack_underflow
{
    stack_underflow(const std::string& msg)
        : msg_(msg) {}

    std::string what() throw() { return msg_; }

private:
    std::string msg_;
};

struct divide_by_zero {};



ForthValue
forth_pop_value(ForthStack& stk, const std::string& callee)
{
    if (stk.empty()) throw stack_underflow(callee);
    ForthValue value = stk.top();
    stk.pop();
    return value;
}

void
word_add(ForthStack& stk)
{
    ForthValue x = forth_pop_value(stk, "+");
    ForthValue y = forth_pop_value(stk, "+");

    stk.push(x + y);
}

void
word_subtract(ForthStack& stk)
{
    ForthValue x = forth_pop_value(stk, "-");
    ForthValue y = forth_pop_value(stk, "-");

    stk.push(x - y);
}

void
word_multiply(ForthStack& stk)
{
    ForthValue x = forth_pop_value(stk, "*");
    ForthValue y = forth_pop_value(stk, "*");

    stk.push(x * y);
}

void
word_divide(ForthStack& stk)
{
    ForthValue x = forth_pop_value(stk, "/");
    ForthValue y = forth_pop_value(stk, "/");

    if (y == 0) throw divide_by_zero();
    stk.push(x / y);
}

void
word_print(ForthStack& stk)
{
    std::cout << forth_pop_value(stk, ".");
}


class ForthInterp
{
private:
    void
    init_words() {
        words_.clear();

        words_["+"] = word_add;
        words_["-"] = word_subtract;
        words_["*"] = word_multiply;
        words_["/"] = word_divide;
        words_["."] = word_print;
    }

public:
    void
    run(const std::string& code) {
        this->init_words();
        std::istringstream ss(code);
        std::string token;

        while (1) {
            ss >> token;
            if (!ss) break;
            // std::cerr << "[debug]:" << token << std::endl;
            if (words_.find(token) != words_.end()) {
                try {
                    words_[token](stack_);
                }
                catch (stack_underflow& e) {
                    std::cerr << e.what() << ": no more items on the stack." << std::endl;
                }
                catch (divide_by_zero& e) {
                    std::cerr << "divide by zero." << std::endl;
                }
            }
            else {
                try {
                    stack_.push(boost::lexical_cast<ForthValue>(token));
                }
                catch (boost::bad_lexical_cast& e) {
                    std::cerr << "can't convert '" << token << "' to integer." << std::endl;
                }
            }
        }
    }

private:
    ForthWordMap words_;
    ForthStack stack_;
};


} // namespace forth


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

    forth::ForthInterp interp;
    interp.run(buf);
}
