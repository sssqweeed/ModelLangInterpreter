#include "Interpreter.h"

#include <string>
#include <stack>
Interpreter::Interpreter(const char* file_name): parser(file_name) { }

void Interpreter::run(){
    if (!parser.analyze()){
        throw "Parser throw";
    }
    parser.print_poliz();
    
    std::stack<Lex> stack;
    auto poliz = parser.poliz;
    
    unsigned long position = 0;
    
    std::cout << "\nInterpteter: \n";
    
    while (position != poliz.size()) {
        type_of_lex type_lex = poliz[position].get_type();
        if (type_lex == LEX_ID or type_lex == LEX_NUM or type_lex == LEX_STRING_DATA or type_lex == LEX_TRUE or type_lex == LEX_FALSE or type_lex == POLIZ_LABEL or type_lex == POLIZ_ADDRESS) {
            stack.push(poliz[position]);
        } else if (type_lex == LEX_PLUS or type_lex == LEX_MINUS or type_lex == LEX_SLASH or type_lex == LEX_TIMES) {
            auto op2 = stack.top();
            stack.pop();
            auto op1 = stack.top();
            stack.pop();
            
            type_of_lex type_ex = get_type_exeption(op1);
            
            if (type_ex == LEX_STRING) {
                std::string s1 = get_string(op1);
                std::string s2 = get_string(op2);
                
                stack.push(Lex(LEX_STRING_DATA, parser.scan.string_data.size()));
                //always LEX_PLUS
                parser.scan.string_data.push_back(s1 + s2);
                
            } else if (type_ex == LEX_INT) { // LEX_INT
                
                long long result, val1 = get_int(op1), val2 = get_int(op2);
                switch (type_lex) {
                    case LEX_PLUS:
                        result = val1 + val2;
                        break;
                    case LEX_MINUS:
                        result = val1 - val2;
                        break;
                    case LEX_SLASH:
                        result = val1 / val2;
                        break;
                    case LEX_TIMES:
                        result = val1 * val2;
                        break;
                    default:
                        throw "Bug";
                        break;
                }
                stack.push(Lex(LEX_NUM, result));
            }
        } else if (type_lex == LEX_BOOL) {
            // never
        } else if (type_lex == POLIZ_GO) {
            auto op = stack.top();
            stack.pop();
            position = op.get_value();
            continue;
        } else if (type_lex == POLIZ_FGO) {
            auto op2 = stack.top();
            stack.pop();
            auto op1 = stack.top();
            stack.pop();
            if (!get_bool(op1)){
                position = op2.get_value();
                continue;
            }
            
        } else if (type_lex == LEX_WRITE) {
            
            auto op = stack.top();
            stack.pop();
            
            type_of_lex type_ex = get_type_exeption(op);
            if (type_ex == LEX_STRING){
                std::cout << "write->" << get_string(op) << std::endl;
            } else if (type_ex == LEX_INT) {
                std::cout << "write->" << get_int(op) << std::endl;
            } else if (type_ex == LEX_BOOL) {
                if (get_bool(op)) {
                    std::cout << "write->" << "TRUE" << std::endl;
                } else {
                    std::cout << "write->" << "FALSE" << std::endl;
                }
            } else {
                throw "Bug";
            }
        } else if (type_lex == LEX_ASSIGN) {
            auto src = stack.top();
            stack.pop();
            auto dst = stack.top();
            stack.pop();
            
            type_of_lex type_ex = get_type_address(dst);
            if (type_ex == LEX_STRING) {
                auto position_string_data = parser.scan.string_data.size();
                parser.scan.string_data.push_back(get_string(src));
                parser.scan.TID[dst.get_value()].put_value(position_string_data);
                stack.push(Lex(LEX_STRING_DATA, position_string_data));
            } else if (type_ex == LEX_INT) {
                long long value = get_int(src);
                parser.scan.TID[dst.get_value()].put_value(value);
                stack.push(Lex(LEX_NUM, value));
            } else if (type_ex == LEX_BOOL) {
                bool value = get_bool(src);
                type_of_lex value_to_stack;
                value ? value_to_stack = LEX_TRUE : value_to_stack = LEX_FALSE;
                parser.scan.TID[dst.get_value()].put_value(value);
                stack.push(Lex(value_to_stack));
            }
        } else if (type_lex == LEX_SEMICOLON) {
            stack.pop();
        } else if (type_lex == LEX_LSS or type_lex == LEX_LEQ or
                   type_lex == LEX_GTR or type_lex == LEX_GEQ or
                   type_lex == LEX_EQ or type_lex == LEX_NEQ) {
            auto op2 = stack.top();
            stack.pop();
            auto op1 = stack.top();
            stack.pop();
            
            type_of_lex type_ex = get_type_exeption(op1);
            
            bool result;
            
            if (type_ex == LEX_STRING) {
                std::string s1 = get_string(op1);
                std::string s2 = get_string(op2);
                result = cmp(type_lex, s1, s2);
            } else {
                long long val1 = get_int(op1), val2 = get_int(op2);
                result = cmp(type_lex, val1, val2);
            }
            
            if (result){
                stack.push(Lex(LEX_TRUE));
            } else {
                stack.push(Lex(LEX_FALSE));
            }
        }
        position++;
    }
    parser.scan.TID;
    parser.scan.string_data;
    std::cout << "\n";
}

long long Interpreter::get_value_op(Lex op){
    long long value;
    if (op.get_type() == LEX_ID) {
        value = parser.scan.TID[op.get_value()].get_value();
    } else if (op.get_type() == LEX_NUM or op.get_type() == LEX_STRING_DATA) {
        value = op.get_value();
    } else {
        throw "Bug";
    }
    return value;
}

type_of_lex Interpreter::get_type_exeption(Lex op){
    if (op.get_type() == LEX_STRING_DATA){
        return LEX_STRING;
    } else if (op.get_type() == LEX_NUM){
        return LEX_INT;
    } else if (op.get_type() == LEX_FALSE or op.get_type() == LEX_TRUE){
        return LEX_BOOL;
    } else {
        return parser.scan.TID[op.get_value()].get_type();
    }
}

type_of_lex Interpreter::get_type_address(Lex op){
    if (op.get_type() != POLIZ_ADDRESS){
        throw "Bug";
    }
    return parser.scan.TID[op.get_value()].get_type();
}

long long Interpreter::get_int(Lex op){
    long long value;
    if (op.get_type() == LEX_ID) {
        value = parser.scan.TID[op.get_value()].get_value();
    } else if (op.get_type() == LEX_NUM) {
        value = op.get_value();
    } else {
        throw "Bug";
    }
    return value;
}

std::string Interpreter::get_string(Lex op){
    std::string result;
    if (op.get_type() == LEX_ID) {
        size_t num_of_row_string_data = parser.scan.TID[op.get_value()].get_value();
        result = parser.scan.string_data[num_of_row_string_data];
    } else if (op.get_type() == LEX_STRING_DATA) {
        size_t num_of_row_string_data = op.get_value();
        result = parser.scan.string_data[num_of_row_string_data];
    } else {
        throw "Bug";
    }
    
    return result;
}

bool Interpreter::get_bool(Lex op){
    if (op.get_type() == LEX_ID) {
        return parser.scan.TID[op.get_value()].get_value();
    } else if (op.get_type() == LEX_FALSE or op.get_type() == LEX_TRUE) {
        return op.get_type() == LEX_TRUE;
    } else {
        throw "Bug";
    }
}

template <class T>
bool Interpreter::cmp(type_of_lex type_cmp, T val1, T val2){
    bool result;
    switch (type_cmp) {
        case LEX_LEQ:
            result = val1 <= val2;
            break;
        case LEX_LSS:
            result = val1 < val2;
            break;
        case LEX_GTR:
            result = val1 > val2;
            break;
        case LEX_GEQ:
            result = val1 >= val2;
            break;
        case LEX_EQ:
            result = val1 == val2;
            break;
        case LEX_NEQ:
            result = val1 != val2;
            break;
        default:
            throw "Bug";
            break;
    }
    return result;
}
