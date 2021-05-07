#include "Parser.h"
#include <string>

Parser::Parser(const char* file_name_): scan(file_name_), file_name(file_name_) {  }

void Parser::get_next_lex(){
    
    curr_lex = scan.get_lex();
    
    c_type = curr_lex.get_type();
    c_val = curr_lex.get_value();
}

bool Parser::analyze(){
    try {
        find_marks();
        
        get_next_lex();
        program();
        if (c_type != LEX_FIN){
            throw "Expected end programm but we have lex with id " + std::to_string(c_type);
        }
        std::cout << "Everything is ok!\n ";
        return true;
    } catch (std::string message) {
        std::cout << message;
        std::cout << curr_lex;
        return false;
    } catch(char c) {
        std::cout << "Unexpected character " << c << "\n";
        return false;
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
    while (c_type != LEX_END) {
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
            throw std::string("Unexpected string declaration ");
        }
        
        // put num of row in string_data
        scan.TID[current_id].put_value(scan.string_data.size() - 1);
        
    } else if (c_type == LEX_MINUS or c_type == LEX_PLUS){
        
        long long value;
        
        if (c_type == LEX_MINUS) {
            value = -1;
        }
        else {
            value = 1;
        }
        
        get_next_lex();
        if (c_type == LEX_NUM){
            
            value = value * c_val;
            
            scan.TID[current_id].put_value(value);
            
            if (type != LEX_INT){
                throw std::string("Unexpected number declaration ");
            }
        } else {
            throw "Expected number but we have lex with id " + std::to_string(c_type);
        }
    } else if (c_type == LEX_NUM){
        if (type != LEX_INT){
            throw std::string("Unexpected number declaration ");
        }
        scan.TID[current_id].put_value(c_val);
        
    } else if (c_type == LEX_FALSE or c_type == LEX_TRUE){
        if (type != LEX_BOOL){
            throw std::string("Unexpected true/false declaration ");
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
     
     E-> A{or A}
     A-> B{and B}
     B-> C [[ == | > | < | !=]  C]
     C-> D {[+ | - ] D}
     D-> F {[ * | / ] F}
     F->…
     
     
     {_I=_}E функции I не существует, вызывается E с поверкой
     возвращаемого значения (распознало ли E именно I)
     
     flag - условие, что выражение находится внутри if() while() и тд.
     например при false закрывается возможность открыть составной оператор
     */
    
    
    // marks def
    size_t l1, l2;
    
    if (flag)
        
        switch (c_type) {
            case LEX_IF:
                
                // if(a) S1 else S2
                //  a  7 !F S1  8  ! S2
                //  1  2  3  4  5  6  7  8
                get_next_lex();
                if (c_type != LEX_LPAREN){
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S(false);
                
                l1 = poliz.size();
                // put to l1 7
                
                poliz.push_back(Lex(POLIZ_LABEL, 0)); // to 7
                poliz.push_back(Lex(POLIZ_FGO, 0));
                
                if(lex_stack.top() != LEX_BOOL){
                    throw std::string("'if' only works with bool type ");
                }
                lex_stack.pop();
                
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S(); // S1
                
                l2 = poliz.size();
                poliz.push_back(Lex(POLIZ_LABEL, 0)); // to 8
                poliz.push_back(Lex(POLIZ_GO, 0));
                
                if (c_type != LEX_ELSE){
                    throw "Expected else but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                poliz[l1] = Lex(POLIZ_LABEL, poliz.size());
                S(); // S2
                poliz[l2] = Lex(POLIZ_LABEL, poliz.size());
                break;
            case LEX_WHILE:
                
                // while(a) S
                //  a  7 !F  S  1  !
                //  1  2  3  4  5  6  7
                
                
                cycle_depth++;
                get_next_lex();
                if (c_type != LEX_LPAREN){
                    throw "Expected '(' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                l1 = poliz.size(); //position_begin
                
                S(false);
                
                l2 = poliz.size(); //position_to_end
                poliz.push_back(Lex(POLIZ_BUF));
                poliz.push_back(Lex(POLIZ_FGO));
                
                if(lex_stack.top() != LEX_BOOL){
                    throw std::string("'while' only works with bool type ");
                }
                lex_stack.pop();
                
                if (c_type != LEX_RPAREN){
                    throw "Expected ')' but we have lex with id " + std::to_string(c_type);
                }
                get_next_lex();
                
                S();
                poliz.push_back(Lex(POLIZ_LABEL, l1));
                poliz.push_back(Lex(POLIZ_GO, l1));
                
                poliz[l2] = Lex(POLIZ_LABEL, poliz.size());
                
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
                        if (i == 1) {
                            throw std::string("'for(a;b;c)' b can't be empty ");
                        }
                    } else {
                        S(false);
                        if (i == 1) {
                            if(lex_stack.top() != LEX_BOOL){
                                throw std::string("'for(a;b;c)' b must be bool type ");
                            }
                        }
                        lex_stack.pop();
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
                
                if (c_type == LEX_ID){
                    auto iter = std::find(marks.begin(), marks.end(), scan.TID[c_val]);
                    
                    if (iter == marks.end()){
                        throw "Undeclarated mark " + std::to_string(c_type);
                    }
                } else {
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
                
                poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
                poliz.push_back(Lex(LEX_READ));
                
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
                lex_stack.pop();
                
                while (c_type == LEX_COMMA) {
                    get_next_lex();
                    S(false);
                    lex_stack.pop();
                }
                
                poliz.push_back(Lex(LEX_WRITE));
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
                        
                        // swap top elem of poliz (ID) to address
                        Lex top_elem = poliz.back();
                        poliz.pop_back();
                        poliz.push_back(Lex(POLIZ_ADDRESS, top_elem.get_value()));
                        
                    } else if (c_type == LEX_COLON) {
                        //ID:operator
                        
                        if (type != mark) {
                            throw "Unexpected ':' after non-mark ID " + std::to_string(c_type);
                        }
                        
                        get_next_lex();
                        S();
                        return;
                    } else if (c_type == LEX_SEMICOLON) {
                        break;
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
                
                // add assingn to poliz
                int cout_assingn = iteration;
                
                while (cout_assingn > 0) {
                    poliz.push_back(Lex(LEX_ASSIGN));
                    cout_assingn--;
                }
                
                poliz.push_back(Lex(LEX_SEMICOLON));
                // check that every type of element match
                type_of_lex type_stack = lex_stack.top();
                while (iteration >= 0) {
                    if (type_stack != lex_stack.top()) {
                        throw std::string("Type error");
                    }
                    lex_stack.pop();
                    
                    iteration--;
                }
                //there isn't type of operation in stack!
                
                get_next_lex();
                break;
        }
    
    else {
        int iteration = 0;
        type_ID type;
        while ((type = E()) == normal and c_type == LEX_ASSIGN) {
            get_next_lex();
            Lex top_elem = poliz.back();
            poliz.pop_back();
            poliz.push_back(Lex(POLIZ_ADDRESS, top_elem.get_value()));
            iteration++;
        }
        
        // add assingn to poliz
        int cout_assingn = iteration;
        
        while (cout_assingn > 0) {
            poliz.push_back(Lex(LEX_ASSIGN));
            cout_assingn--;
        }
        
        type_of_lex type_stack = lex_stack.top();
        while (iteration >= 0) {
            if (type_stack != lex_stack.top()) {
                throw std::string("Type error ");
            }
            lex_stack.pop();
            
            iteration--;
        }
        
        lex_stack.push(type_stack);
        //there is type of operation in stack!
    }
}

Parser::type_ID Parser::E(){
    type_ID type = A();
    
    std::stack<size_t> positions;

    if (c_type == LEX_OR){
        // add if false goto next
        size_t position_next = poliz.size();
        poliz.push_back(Lex(POLIZ_BUF));
        poliz.push_back(Lex(POLIZ_FGO));

        // add goto end
        //poliz.push_back(Lex(LEX_TRUE));
        positions.push(poliz.size());
        poliz.push_back(Lex(POLIZ_BUF));
        poliz.push_back(Lex(POLIZ_GO));
        poliz[position_next] = Lex(POLIZ_LABEL, poliz.size());
        while (c_type == LEX_OR) {
            
            lex_stack.push(c_type);
            
            get_next_lex();
            
            A();
            
            // add if false goto next
            position_next = poliz.size();
            poliz.push_back(Lex(POLIZ_BUF));
            poliz.push_back(Lex(POLIZ_FGO));

            // add goto end, true
            //poliz.push_back(Lex(LEX_TRUE));
            positions.push(poliz.size());
            poliz.push_back(Lex(POLIZ_BUF));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[position_next] = Lex(POLIZ_LABEL, poliz.size());

            check_types();
            
            type = not_reference_to_ID;
        }
        // add false
        poliz.push_back(Lex(LEX_FALSE));
        // goto end
        size_t position_insert_end = poliz.size();
        poliz.push_back(Lex(POLIZ_BUF));
        poliz.push_back(Lex(POLIZ_GO));
        
        poliz[position_insert_end] = Lex(POLIZ_LABEL, poliz.size());
        push_to_poliz(positions, poliz.size());
        
        poliz.push_back(Lex(LEX_TRUE));
        
        
    }
    return type;
}

void Parser::push_to_poliz(std::stack<size_t>& positions, unsigned long position){
    while (!positions.empty()) {
        poliz[positions.top()] = Lex(POLIZ_LABEL, position);
        positions.pop();
    }
}

Parser::type_ID Parser::A(){
    
    std::stack<size_t> positions;
    
    type_ID type = B();
    
    if (c_type == LEX_AND) {
        //lazy evaluation
        positions.push(poliz.size());
        poliz.push_back(Lex(POLIZ_BUF));
        poliz.push_back(Lex(POLIZ_FGO));
        // add goto end with false
        while (c_type == LEX_AND) {
            
            lex_stack.push(c_type);
            
            get_next_lex();
            B();
            
            //lazy evaluation
            positions.push(poliz.size());
            poliz.push_back(Lex(POLIZ_BUF));
            poliz.push_back(Lex(POLIZ_FGO));
            
            check_types();
            
            type = not_reference_to_ID;
        }
        //add true and goto next
        
        //lazy evaluation
        poliz.push_back(Lex(LEX_TRUE));
        size_t position_goto_next = poliz.size();
        poliz.push_back(Lex(POLIZ_BUF));
        poliz.push_back(Lex(POLIZ_GO));
        
        unsigned long position_false = poliz.size();
        push_to_poliz(positions, position_false);
        
        poliz.push_back(Lex(LEX_FALSE));
        poliz[position_goto_next] = Lex(POLIZ_LABEL, poliz.size());
    }
    return type;
}

Parser::type_ID Parser::B(){
    type_ID type = C();
    if (c_type == LEX_EQ or c_type == LEX_NEQ or c_type == LEX_GEQ or c_type == LEX_LEQ or c_type == LEX_LSS or c_type == LEX_GTR) {
        
        lex_stack.push(c_type);
        
        get_next_lex();
        C();
        
        check_types();
        
        type = not_reference_to_ID;
    }
    
    return type;
}
Parser::type_ID Parser::C(){
    type_ID type = D();
    while (c_type == LEX_MINUS or c_type == LEX_PLUS) {
        
        lex_stack.push(c_type);
        
        get_next_lex();
        D();
        
        check_types();
        
        type = not_reference_to_ID;
    }
    
    return type;
}
Parser::type_ID Parser::D(){
    type_ID type = F();
    while (c_type == LEX_TIMES or c_type == LEX_SLASH) {
        
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
        //poliz
        
        poliz.push_back(Lex(LEX_ID, c_val));
        //___
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
        //poliz
        poliz.push_back(Lex(LEX_NUM, c_val));
        //___
        lex_stack.push(LEX_INT);
        get_next_lex();
    } else if (c_type == LEX_TRUE) {
        //poliz
        poliz.push_back(Lex(LEX_TRUE));
        //___
        lex_stack.push(LEX_BOOL);
        get_next_lex();
    } else if (c_type == LEX_FALSE) {
        //poliz
        poliz.push_back(Lex(LEX_FALSE));
        //___
        
        lex_stack.push(LEX_BOOL);
        get_next_lex();
    } else if (c_type == LEX_STRING_DATA) {
        //poliz
        auto num_of_row = scan.string_data.size() - 1;
        poliz.push_back(Lex(LEX_STRING_DATA, num_of_row));
        //___
        lex_stack.push(LEX_STRING);
        get_next_lex();
    } else if (c_type == LEX_NOT) {
        get_next_lex();
        F();
        //check not
        //poliz
        poliz.push_back(Lex(LEX_NOT));
        //___
        if (lex_stack.top() != LEX_BOOL){
            throw std::string("'Not' operation is not applicable to this type ");
        }
    } else if (c_type == LEX_LPAREN) {
        get_next_lex();
        E();
        if (c_type == LEX_RPAREN) {
            get_next_lex();
        } else {
            throw "Expected ')' but we have lex with id " + std::to_string(c_type);;
        }
    } else if (c_type == LEX_MINUS or c_type == LEX_PLUS) {
        get_next_lex();
        if (c_type == LEX_ID or c_type == LEX_NUM or c_type == LEX_LPAREN) {
            poliz.push_back(Lex(LEX_NUM, 0));
            F();
            poliz.push_back(Lex(LEX_MINUS, 0));
        } else {
            throw "Expected id or number but we have lex with id " + std::to_string(c_type);
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
    
    
    //poliz:
    if (operation != LEX_OR and operation != LEX_AND) {
        poliz.push_back(Lex(operation));
    }
    //_____
    
    type_of_lex type_of_op = first_op;
    
    // types     : string int bool
    // operations: + - * / > < >= <= == != and or
    // not in F()
    
    // op & string + == !=
    // op & int + - * / > < >= <= == !=
    // op & bool and or
    
    if(first_op != sec_op){
        throw std::string("Type error ");
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

void Parser::print_poliz(){
    for (Lex curr_elem : poliz) {
        std::cout << curr_elem << " ";
    }
}
