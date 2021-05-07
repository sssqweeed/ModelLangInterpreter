#include "Parser.h"


class Interpreter{
private:
    Parser parser;
    
    long long get_value_op(Lex op);
    type_of_lex get_type_exeption(Lex op);
    type_of_lex get_type_address(Lex op);
    
    long long get_int(Lex op);
    std::string get_string(Lex op);
    bool get_bool(Lex op);

    template <class T>
    bool cmp(type_of_lex type_cmp, T op1, T op2);
    
public:
    explicit Interpreter(const char* file_name);
    void run();
};
