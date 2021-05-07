#pragma once
#include <iostream>

enum type_of_lex {
    LEX_NULL,
    LEX_AND, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT,
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_TRUE, LEX_WHILE, LEX_FOR, LEX_WRITE, LEX_STRING, LEX_STRUCT,
    LEX_GOTO, LEX_BREAK,
    LEX_FIN,
    LEX_BEGIN, LEX_END, LEX_SEMICOLON, LEX_COMMA, LEX_EQ, LEX_LPAREN, LEX_RPAREN, LEX_ASSIGN, LEX_LSS,
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_COLON,
    LEX_NUM,
    LEX_ID,
    LEX_STRING_DATA,
    POLIZ_GO, POLIZ_FGO, POLIZ_ADDRESS, POLIZ_LABEL, POLIZ_BUF
};

class Lex{
private:
    type_of_lex t_lex;
    long long v_lex;
    long num_row_in_file;
public:
    Lex(type_of_lex t = LEX_NULL, long long v = 0, long _num_row_in_file = 0);
    
    type_of_lex get_type() const;
    
    unsigned long get_value() const;
    
    friend std::ostream & operator<<(std::ostream &s, Lex l);
};
