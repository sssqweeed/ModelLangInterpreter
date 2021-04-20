#pragma once
#include <string>
#include "Lex.h"

class Ident {
    std::string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
    
public:
    Ident();
    
    Ident(const std::string n);
    
    bool operator==(const std::string& s) const ;
    
    std::string get_name() const;
    
    bool get_declare() const;
    
    void put_declare();
    
    type_of_lex get_type() const;
    
    void put_type(type_of_lex t);
    
    bool get_assign() const;
    
    void put_assign();
    
    int get_value()const;
    
    void put_value(int v);
};
