#pragma once
#include "Scanner.h"

class Parser{
private:
    
    Lex curr_lex; // текущая лексема
    type_of_lex c_type;
    unsigned long c_val;
    Scanner scan;
    
    void get_next_lex();
    
    void program();
    void definitions();
    void operators();
    void definition();
    void type();
    void variable();
    void id();
    void constant();
    void constant_string();
    void constant_int();
    
public:
    
    void analyze();
};
