#pragma once
#include "Scanner.h"
#include <vector>
#include <stack>

class Parser{
public:
    
    enum type_ID{
        normal, mark, undeclared, not_reference_to_ID
    };
    
    Lex curr_lex;
    type_of_lex c_type;
    unsigned long c_val;
    type_of_lex type_def;
    Scanner scan;
    unsigned long current_id;
    int cycle_depth;
    const char* file_name;
    
    std::stack<type_of_lex> lex_stack;
    std::vector<Ident> marks;
    
    type_of_lex get_cur_type_def() const;
    
    void set_type_def(const type_of_lex type);
    
    bool is_redefinition(Lex lex);
    
    void find_marks();
    
    void get_next_lex();
    
    void program();
    
    void definitions();
    
    void operators();
    
    void variable();
    
    void constant();
    
    void S(bool flag = true);
    
    type_ID E();
    
    type_ID E1();
    
    type_ID T();
    
    type_ID F();
    
public:
    Parser(const char* file_name_);
    
    void analyze();
};
