#pragma once
#include "Scanner.h"
#include <vector>
#include <stack>

class Parser{
private:
    
    enum type_ID{
        normal, mark, undeclared, not_reference_to_ID
    };
    
    Lex curr_lex;
    type_of_lex c_type;
    unsigned long c_val;
    type_of_lex type_def;
    
    unsigned long current_id;
    int cycle_depth;
    const char* file_name;
    
    std::stack<type_of_lex> lex_stack;
    std::vector<Ident> marks;
    std::vector<size_t> break_to_position;
    std::vector<size_t> marks_positions_in_poliz;
    
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
    type_ID A();
    type_ID B();
    type_ID C();
    type_ID D();
    type_ID F();
    
    bool check_types();
    
    void push_to_poliz(std::stack<size_t>& positions, unsigned long position);
    
    void pop_break_positions();
    
    void pop_goto_positions();
public:
    Parser(const char* file_name_);
    Scanner scan;
    std::vector<Lex> poliz;
    
    void print_poliz();
    
    bool analyze();
};
