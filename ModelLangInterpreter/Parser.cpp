#include "Parser.h"
#include <string>
void Parser::get_next_lex(){
    try{
    curr_lex = scan.get_lex();
    } catch(...) {
        
    }
    c_type = curr_lex.get_type();
    c_val = curr_lex.get_value();
}

void Parser::analyze(){
    try {
        program();
    } catch (char* message) {
        std::cout << message;
    }
}

void Parser::program(){
    get_next_lex();
    if (c_type != LEX_PROGRAM){
        throw "Expected 'program' but we have lex with id"+ std::to_string(c_type);
    }
}
