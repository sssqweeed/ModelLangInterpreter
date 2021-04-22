#include "Parser.h"
#include <string>

Parser::Parser(const char* file_name): scan(file_name) {  }

void Parser::get_next_lex(){
    try{
    curr_lex = scan.get_lex();
    } catch(char c) {
        std::cout << "Unexpected character " << c << "\n";
    }
    c_type = curr_lex.get_type();
    c_val = curr_lex.get_value();
}

void Parser::analyze(){
    try {
        get_next_lex();
        program();
        if (c_type != LEX_FIN){
            throw "Expected end programm but we have lex with id " + std::to_string(c_type);
        }
        
        std::cout << "Everything is ok!\n";
    } catch (std::string message) {
        std::cout << message;
        std::cout << curr_lex;
    }
}

void Parser::program(){
    if (c_type != LEX_PROGRAM){
        throw "Expected 'program' but we have lex with id " + std::to_string(c_type);
    }
    else{
        get_next_lex();
        if (c_type != LEX_BEGIN){
            throw "Expected '{' but we have lex with id " + std::to_string(c_type);
        }
        get_next_lex();
        definitions();
        
        operators();
        
        //get_next_lex();
        if (c_type != LEX_END){
            throw "Expected '}' but we have lex with id " + std::to_string(c_type);
        }
        get_next_lex();
    }
}

void Parser::definitions(){
    
    while (c_type == LEX_INT or c_type == LEX_BOOL or
           c_type == LEX_STRING or c_type == LEX_STRUCT) {
        get_next_lex();
        variable();
        
        while (c_type == LEX_COMMA) {
            get_next_lex();
            variable();
        }
        
        if (c_type != LEX_SEMICOLON){
            throw "Expected ';' but we have lex with id " + std::to_string(c_type);
        }
        get_next_lex();
    }
}

void Parser::operators(){
    while (c_type == LEX_IF or c_type == LEX_ELSE or c_type == LEX_WHILE or c_type == LEX_FOR or
           c_type == LEX_BREAK or c_type == LEX_GOTO or c_type == LEX_READ or
           c_type == LEX_WRITE or c_type == LEX_IF or c_type == LEX_ID) {
        
        S();
        //get_next_lex();
    }
}



void Parser::S(bool flag){
    
    /*
     {_ a _} означает a^n n >= 0
     ------------------------
     operators -> {_S_}
     
     S->
     {_I=_}E |
     if (S(false) S else S |
     while (S(false)) S|
     read (интетификатор); |
     write (S(false) {_, S(false)_} ); |
     составной оператор |
     оператор-выражение
     
     E, E1 и тд означают то, что в методичке
     
     {_I=_}E функции I не существует, вызывается E с поверкой
     возвращаемого значения (распознало ли E именно I)
     
     flag - условие, что выражение находится внутри if() while() и тд.
     например при false закрывается возможность открыть составной оператор
     */
    
    if (flag)
        switch (c_type) {
            case LEX_IF:
                get_next_lex();
                if (c_type != LEX_LPAREN){
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S(false);
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S();
                
                if (c_type != LEX_ELSE){
                    throw "Expected else but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S();
                break;
            case LEX_WHILE:
                get_next_lex();
                if (c_type != LEX_LPAREN){
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S(false);
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S();
                
                break;
            case LEX_FOR:
                get_next_lex();
                if (c_type != LEX_LPAREN){
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                for (int i = 0; i < 2; i++) {
                    if (c_type == LEX_SEMICOLON) {
                        get_next_lex();
                    } else {
                        S(false);
                        if (c_type != LEX_SEMICOLON) {
                            throw "Expected ';' but we have lex with id " + std::to_string(c_type);
                        }
                        get_next_lex();
                    }
                    
                }
                
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S();
                
                break;
            case LEX_BREAK:
                get_next_lex();
                
                if (c_type != LEX_SEMICOLON){
                    throw "Expected ';' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                break;
            case LEX_GOTO:
                get_next_lex();
                
                if (c_type != LEX_ID){
                    throw "Expected ID but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                if (c_type != LEX_SEMICOLON){
                    throw "Expected ';' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                break;
            case LEX_READ:
                get_next_lex();
                if (c_type != LEX_LPAREN) {
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                
                if (c_type != LEX_ID){
                    throw "Expected ID but we have lex with id " + std::to_string(c_type) + "\n";
                }
                get_next_lex();
                
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                
                get_next_lex();
                
                if (c_type != LEX_SEMICOLON){
                    throw "Expected ';' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                break;
                
            case LEX_WRITE:
                get_next_lex();
                if (c_type != LEX_LPAREN) {
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                
                S(false);
                
                while (c_type == LEX_COMMA) {
                    get_next_lex();
                    S(false);
                }
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                
                get_next_lex();
                
                if (c_type != LEX_SEMICOLON){
                    throw "Expected ';' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                break;
            case LEX_BEGIN:
                get_next_lex();
                operators();
                
                if (c_type != LEX_END){
                    throw "Expected '}' but we have lex with id " + std::to_string(c_type);
                }
                
                get_next_lex();
                
                break;
                
            default:
                // {_E=_}E
                bool result_is_id;
                for (int iteration = 0; (result_is_id = E()); iteration++) {
                    
                    if (c_type == LEX_ASSIGN) {
                        //E=E=E...
                    } else if (c_type == LEX_COLON) {
                        //ID:operator
                        get_next_lex();
                        S();
                        return;
                    } else {
                        if (iteration > 0)
                            throw "Expected '=' but we have lex with id " + std::to_string(c_type);
                        else
                            throw "Expected '=' or ':' but we have lex with id " + std::to_string(c_type);
                    }
                    get_next_lex();
                }
                
                if (c_type != LEX_SEMICOLON){
                    throw "Expected ';' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                break;
        }
    
    else {
        bool result_is_id;
        while ((result_is_id = E()) and c_type == LEX_ASSIGN) {
            get_next_lex();
        }
    }
}



bool Parser::E(){
    bool is_id = E1();
    if (c_type == LEX_EQ or c_type == LEX_NEQ or c_type == LEX_GEQ or c_type == LEX_LEQ or c_type == LEX_LSS or c_type == LEX_GTR) {
        get_next_lex();
        E1();
        
        is_id = false;
    }
    
    return is_id;
}

bool Parser::E1(){
    bool is_id = T();
    while (c_type == LEX_PLUS or c_type == LEX_MINUS or c_type == LEX_OR){
        get_next_lex();
        T();
        
        is_id = false;
    }
    
    return is_id;
}

bool Parser::T(){
    bool is_id = F();
    while (c_type == LEX_SLASH or c_type == LEX_AND or c_type == LEX_TIMES) {
        get_next_lex();
        F();
        
        is_id = false;
    }
    
    return is_id;
}

bool Parser::F(){
    bool is_id = false;
    if (c_type == LEX_ID) {
        get_next_lex();
        is_id = true;
    } else if (c_type == LEX_NUM) {
        get_next_lex();
    } else if (c_type == LEX_TRUE) {
        get_next_lex();
    } else if (c_type == LEX_FALSE) {
        get_next_lex();
    } else if (c_type == LEX_STRING_DATA) {
            get_next_lex();
    } else if (c_type == LEX_NOT) {
        get_next_lex();
        F ();
    } else if (c_type == LEX_LPAREN) {
        get_next_lex();
        E();
        if (c_type == LEX_RPAREN) {
            get_next_lex();
        } else {
            throw "Expected ')' but we have lex with id " + std::to_string(c_type);;
        }
    }
    
    return is_id;
}

void Parser::variable(){
    if (c_type != LEX_ID) {
        throw "Expected ID but we have lex with id " + std::to_string(c_type);
    }
    get_next_lex();
    if (c_type == LEX_ASSIGN) {
        get_next_lex();
        constant();
        get_next_lex();
    }
}

void Parser::constant(){
    if (c_type == LEX_STRING_DATA){
        
    } else if (c_type == LEX_MINUS or c_type == LEX_PLUS){
        get_next_lex();
        if (c_type == LEX_NUM){
            
        }
    } else if (c_type == LEX_NUM){
        
    } else if (c_type == LEX_FALSE or c_type == LEX_TRUE){
        
    } else {
        throw "Expected constant but we have lex with id " + std::to_string(c_type);
    }
}
