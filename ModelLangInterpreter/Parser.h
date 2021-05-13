#pragma once
#include "Scanner.h"
#include <vector>
#include <stack>

class Parser{
private:
    
    struct Field_of_struct{
        std::string name;
        type_of_lex type;
        
        Field_of_struct(std::string _name, type_of_lex _type);
    };
    
    struct Struct{
        std::string name;
        std::vector<Field_of_struct> fields;
        Struct(std::string name, const std::vector<Field_of_struct>& fields = {});
    };
    
    enum type_ID{
        normal, mark, undeclared, not_reference_to_ID
    };
    
    Lex curr_lex;
    type_of_lex c_type;
    long long c_val;
    type_of_lex type_def;
    long long num_struct;
    long long current_id;
    int cycle_depth;
    const char* file_name;
    
    std::stack<type_of_lex> lex_stack;
    std::vector<Ident> marks;
    std::vector<size_t> break_to_position;
    std::vector<size_t> marks_positions_in_poliz;
    
    type_of_lex get_cur_type_def() const;
    
    void set_type_def(const type_of_lex type);
    
    bool is_redefinition(Lex lex);
    
    bool is_redefinition(std::string name, const std::vector<Field_of_struct>& fields);
    
    bool is_correct_name();
    
    void find_marks();
    
    void get_next_lex();
    
    void program();
    
    void definitions();
    
    void definition_of_structures();
    
    bool is_struct_name();
    
    void m_alloc_for_struct();
    
    std::vector<Field_of_struct> definition_of_structure_fields();
    
    void field();
    
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
    std::vector<Struct> structures;
    
    void print_poliz();
    
    bool analyze();
};
