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




struct ForthInterpException {
    ForthInterpException(const std::string& msg)
        : msg_(msg) {}

    std::string what() throw() { return msg_; }

private:
    std::string msg_;
};

struct StackUnderflow
    : public ForthInterpException
{
    StackUnderflow(const std::string& msg)
        : ForthInterpException(msg + ": no more items on the stack.") {}
};

struct DivideByZero
    : public ForthInterpException
{
    DivideByZero()
        : ForthInterpException("divide by zero.") {}
};



ForthValue
forth_pop_value(ForthStack& stk, const std::string& callee)
{
    if (stk.empty()) throw StackUnderflow(callee);
    ForthValue value = stk.top();
    stk.pop();
    return value;
}

void
word_add(ForthStack& stk)
{
    ForthValue y = forth_pop_value(stk, "+");
    ForthValue x = forth_pop_value(stk, "+");

    stk.push(x + y);
}

void
word_subtract(ForthStack& stk)
{
    ForthValue y = forth_pop_value(stk, "-");
    ForthValue x = forth_pop_value(stk, "-");

    stk.push(x - y);
}

void
word_multiply(ForthStack& stk)
{
    ForthValue y = forth_pop_value(stk, "*");
    ForthValue x = forth_pop_value(stk, "*");

    stk.push(x * y);
}

void
word_divide(ForthStack& stk)
{
    ForthValue y = forth_pop_value(stk, "/");
    ForthValue x = forth_pop_value(stk, "/");

    if (y == 0) throw DivideByZero();
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

            if (token == "(") {
                // forth's comment. skip until ")".
                while (1) {
                    ss >> token;
                    if (!ss) {
                        std::cerr << "[warning]: got EOF inside comment." << std::endl;
                        goto end;
                    }
                    if (token == ")") break;
                }
            }
            else if (words_.find(token) != words_.end()) {
                try {
                    words_[token](stack_);
                }
                catch (ForthInterpException& e) {
                    std::cerr << "[error]: " << e.what() << std::endl;
                }
            }
            else {
                try {
                    stack_.push(boost::lexical_cast<ForthValue>(token));
                }
                catch (boost::bad_lexical_cast& e) {
                    std::cerr << "[error]: can't convert '" << token << "' to integer." << std::endl;
                }
            }
        }
end:
        ;    // it's the end of .run()
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
