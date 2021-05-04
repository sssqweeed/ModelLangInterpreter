#include "Parser.h"
#include <string>

Parser::Parser(const char* file_name_): scan(file_name_), file_name(file_name_) {  }

void Parser::get_next_lex(){
    
    curr_lex = scan.get_lex();
    
    c_type = curr_lex.get_type();
    c_val = curr_lex.get_value();
}

void Parser::analyze(){
    try {
        find_marks();
        
        get_next_lex();
        program();
        if (c_type != LEX_FIN){
            throw "Expected end programm but we have lex with id " + std::to_string(c_type);
        }
        
        std::cout << "Everything is ok!\n";
    } catch (std::string message) {
        std::cout << message;
        std::cout << curr_lex;
    } catch(char c) {
        std::cout << "Unexpected character " << c << "\n";
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
        
        if (c_type != LEX_END){
            throw "Expected '}' but we have lex with id " + std::to_string(c_type);
        }
        get_next_lex();
    }
}

void Parser::definitions(){
    while (c_type == LEX_INT or c_type == LEX_BOOL or
           c_type == LEX_STRING or c_type == LEX_STRUCT) {
        
        set_type_def(c_type);
        
        get_next_lex();
        variable();
        
        while (c_type == LEX_COMMA) {
            get_next_lex();
            variable();
        }
        
        if (c_type != LEX_SEMICOLON) {
            throw "Expected ';' but we have lex with id " + std::to_string(c_type);
        }
        get_next_lex();
    }
}

void Parser::variable(){
    if (c_type != LEX_ID) {
        throw "Expected ID but we have lex with id " + std::to_string(c_type);
    }
    
    if (is_redefinition(curr_lex)){
        throw "Redefinition ID with num in TID " + std::to_string(c_val);
    }
    
    scan.TID[c_val].put_declare();
    scan.TID[c_val].put_type(get_cur_type_def());
    
    current_id = c_val;
    
    get_next_lex();
    if (c_type == LEX_ASSIGN) {
        scan.TID[current_id].put_assign();
        get_next_lex();
        constant();
        get_next_lex();
    }
    else {
        scan.TID[current_id].put_value(-1);
    }
}

void Parser::operators(){
    while (c_type == LEX_IF or c_type == LEX_ELSE or c_type == LEX_WHILE or c_type == LEX_FOR or
           c_type == LEX_BREAK or c_type == LEX_GOTO or c_type == LEX_READ or
           c_type == LEX_WRITE or c_type == LEX_IF or c_type == LEX_ID or c_type == LEX_LPAREN) {
        
        S();
    }
}

bool Parser::is_redefinition(Lex lex){
    auto num_in_TID = lex.get_value();
    if (num_in_TID == scan.TID.size() - 1){
        return false;
    } else {
        return true;
    }
}

void Parser::constant(){
    
    auto type = get_cur_type_def();
    
    if (c_type == LEX_STRING_DATA){
        if (type != LEX_STRING){
            throw std::string("Unexpected string declaration");
        }
        
        scan.TID[current_id].put_value(scan.string_data.size());
        
    } else if (c_type == LEX_MINUS or c_type == LEX_PLUS){
        get_next_lex();
        
        long long value;
        
        if (c_type == LEX_MINUS) {
            value = -1;
        }
        else {
            value = 1;
        }
        
        if (c_type == LEX_NUM){
            
            value = value * c_val;
            
            scan.TID[current_id].put_value(value);
            
            if (type != LEX_INT){
                throw std::string("Unexpected number declaration");
            }
        } else {
            throw "Expected number but we have lex with id " + std::to_string(c_type);
        }
    } else if (c_type == LEX_NUM){
        if (type != LEX_INT){
            throw std::string("Unexpected number declaration");
        }
        scan.TID[current_id].put_value(c_val);
        
    } else if (c_type == LEX_FALSE or c_type == LEX_TRUE){
        if (type != LEX_BOOL){
            throw std::string("Unexpected true/false declaration");
        }
        
        if (c_type == LEX_FALSE) {
            scan.TID[current_id].put_value(0);
        }
        else {
            scan.TID[current_id].put_value(1);
        }
        
    } else {
        throw "Expected constant but we have lex with id " + std::to_string(c_type);
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
                cycle_depth++;
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
                cycle_depth--;
                break;
            case LEX_FOR:
                cycle_depth++;
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
                cycle_depth--;
                break;
            case LEX_BREAK:
                
                if (cycle_depth == 0) {
                    throw "Unexpected 'break' " + std::to_string(c_type);
                }
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
                type_ID type;
                int iteration;
                
                for (iteration = 0; (type = E()) != not_reference_to_ID; iteration++) {
                    
                    if (type == undeclared) {
                        throw "Undeclared ID " + std::to_string(c_type);
                    }
                    
                    if (c_type == LEX_ASSIGN) {
                        //E=E=E...
                        if (type != normal){
                            throw std::string("Unexpected assign ");
                        }
                    } else if (c_type == LEX_COLON) {
                        //ID:operator
                        
                        if (type != mark) {
                            throw "Unexpected ':' after non-mark ID " + std::to_string(c_type);
                        }
                        
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
                
                // check that every type of element match
                
                type_of_lex type_stack = lex_stack.top();
                while (iteration >= 0) {
                    if (type_stack != lex_stack.top()) {
                        throw std::string("Type error");
                    }
                    lex_stack.pop();
                    
                    iteration--;
                }
                
                get_next_lex();
                break;
        }
    
    else {
        type_ID type;
        while ((type = E()) == normal and c_type == LEX_ASSIGN) {
            get_next_lex();
        }
    }
}



Parser::type_ID Parser::E(){
    type_ID type = E1();
    if (c_type == LEX_EQ or c_type == LEX_NEQ or c_type == LEX_GEQ or c_type == LEX_LEQ or c_type == LEX_LSS or c_type == LEX_GTR) {
        
        lex_stack.push(c_type);
        
        get_next_lex();
        E1();
        
        check_types();
        
        type = not_reference_to_ID;
    }
    
    return type;
}

Parser::type_ID Parser::E1(){
    type_ID type = T();
    while (c_type == LEX_PLUS or c_type == LEX_MINUS or c_type == LEX_OR){
        lex_stack.push(c_type);
        get_next_lex();
        T();
        check_types();
        type = not_reference_to_ID;
    }
    
    return type;
}

Parser::type_ID Parser::T(){
    type_ID type = F();
    while (c_type == LEX_SLASH or c_type == LEX_AND or c_type == LEX_TIMES) {
        lex_stack.push(c_type);
        get_next_lex();
        F();
        check_types();
        type = not_reference_to_ID;
    }
    
    return type;
}

Parser::type_ID Parser::F(){
    type_ID type = not_reference_to_ID;
    if (c_type == LEX_ID) {
        if(!scan.TID[c_val].get_declare()){
            // mark?
            auto iterator = std::find(marks.begin(), marks.end(), scan.TID[c_val]);
            if (iterator != marks.end()){
                type = mark;
            } else {
                type = undeclared;
            }
        } else {
            type = normal;
        }
        // push type of ID
        lex_stack.push(scan.TID[c_val].get_type());
        
        get_next_lex();
    } else if (c_type == LEX_NUM) {
        lex_stack.push(LEX_INT);
        get_next_lex();
    } else if (c_type == LEX_TRUE) {
        lex_stack.push(LEX_BOOL);
        get_next_lex();
    } else if (c_type == LEX_FALSE) {
        lex_stack.push(LEX_BOOL);
        get_next_lex();
    } else if (c_type == LEX_STRING_DATA) {
        lex_stack.push(LEX_STRING);
        get_next_lex();
    } else if (c_type == LEX_NOT) {
        //check not
        if (lex_stack.top() != LEX_BOOL){
            throw std::string("'Not' operation is not applicable to this type");
        }
        get_next_lex();
        F();
    } else if (c_type == LEX_LPAREN) {
        get_next_lex();
        E();
        if (c_type == LEX_RPAREN) {
            get_next_lex();
        } else {
            throw "Expected ')' but we have lex with id " + std::to_string(c_type);;
        }
    } else {
        throw "Unxpected lex with id " + std::to_string(c_type);
    }
    
    return type;
}

type_of_lex Parser::get_cur_type_def() const {
    return type_def;
}

void Parser::set_type_def(const type_of_lex type){
    type_def = type;
}


void Parser::find_marks(){
    Scanner scan(file_name);
    type_of_lex cur_lex = scan.get_lex().get_type(), prev_lex;
    
    //empty file
    if(cur_lex == LEX_FIN){
        return;
    }
    
    prev_lex = cur_lex;
    
    while((cur_lex = scan.get_lex().get_type()) != LEX_FIN){
        if(cur_lex == LEX_COLON and prev_lex == LEX_ID){
            
            auto iter = std::find(marks.begin(), marks.end(), scan.TID[scan.TID.size() - 1]);
            
            if (iter != marks.end()){
                throw std::string("Identical marks found ");
            }
            
            marks.push_back(scan.TID[scan.TID.size() - 1]);
        }
        prev_lex = cur_lex;
    }
    
    return;
}

bool Parser::check_types(){
    type_of_lex sec_op = lex_stack.top();
    lex_stack.pop();
    type_of_lex operation = lex_stack.top();
    lex_stack.pop();
    type_of_lex first_op = lex_stack.top();
    lex_stack.pop();
    
    type_of_lex result;
    
    
    type_of_lex type_of_op = first_op;
    
    // types     : string int bool
    // operations: + - * / > < >= <= == != and or
    // not in F()
    
    // op & string + == !=
    // op & int + - * / > < >= <= == !=
    // op & bool and or
    
    if(first_op != sec_op){
        throw std::string("Type error");
    }
    
    if (type_of_op == LEX_STRING){
        if(operation == LEX_PLUS){
            result = LEX_STRING;
        } else if (operation == LEX_NEQ or
                   operation == LEX_EQ){
            result = LEX_BOOL;
        } else {
            throw std::string("Invalid operation with string type ");
        }
    } else if (type_of_op == LEX_INT){
        if(operation == LEX_PLUS or operation == LEX_MINUS or
           operation == LEX_TIMES or operation == LEX_SLASH){
            result = LEX_INT;
        } else if (operation == LEX_LSS or operation == LEX_GTR or
                   operation == LEX_GEQ or operation == LEX_LEQ or
                   operation == LEX_EQ or operation == LEX_NEQ){
            result = LEX_BOOL;
        } else {
            throw std::string("Invalid operation with int type ");
        }
    } else if (type_of_op == LEX_BOOL){
        if(operation == LEX_OR or operation == LEX_AND){
            result = LEX_BOOL;
        } else {
            throw std::string("Invalid operation with bool type ");
        }
    } else {
        throw std::string("Invalid type ");
    }
    
    lex_stack.push(result);
    
    return false;
}
