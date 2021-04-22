#pragma once
#include <string>
#include <vector>
#include "Lex.h"
#include "Ident.h"

class Scanner {
private:
    FILE * fp;
    char c;
    
    long num_of_row_in_file;
    /// Search for buf in the list
    int look(const std::string buf, const char** list);
    
    void get_next_char_from_file();
    unsigned long put(const std::string & buf);

    static std::vector<Ident> TID;
    
    friend std::ostream & operator<<(std::ostream &s, Lex l);
    
public:
    static const char* TW [];
    static const char* TD [];
    std::vector<std::string> string_data;
    
    Scanner(const char* program);
    
    Lex get_lex();
};
 
std::ostream & operator<<(std::ostream &s, Lex l);

 
