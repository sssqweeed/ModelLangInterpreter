#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
 
using namespace std;

enum type_of_lex {
    LEX_NULL,
    LEX_AND, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT,
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_TRUE, LEX_WHILE, LEX_WRITE, LEX_STRING, LEX_STRUCT,
    LEX_FIN,
    LEX_BEGIN, LEX_END, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_LPAREN, LEX_RPAREN, LEX_ASSIGN, LEX_LSS,
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,
    LEX_NUM,
    LEX_ID,
    LEX_STRING_DATA
};

class Lex{
private:
    type_of_lex t_lex;
    unsigned long v_lex;
    
public:
    Lex(type_of_lex t = LEX_NULL, unsigned long v = 0): t_lex(t), v_lex(v) {  }
    
    type_of_lex get_type() const {
        return t_lex;
    }
    
    unsigned long get_value() const {
        return v_lex;
    }
    
    friend ostream & operator<<(ostream &s, Lex l);
};

class Ident {
    string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
    
public:
    Ident(){
        declare = false;
        assign  = false;
    }
    
    Ident(const string n){
        name    = n;
        declare = false;
        assign  = false;
    }
    
    bool operator==(const string& s) const {
        return name == s;
    }
    
    string get_name() const {
        return name;
    }
    bool get_declare() const {
        return declare;
    }
    void put_declare(){
        declare = true;
    }
    type_of_lex get_type() const {
        return type;
    }
    void put_type(type_of_lex t){
        type = t;
    }
    bool get_assign() const {
        return assign;
    }
    void put_assign(){
        assign = true;
    }
    int get_value()const {
        return value;
    }
    void put_value(int v){
        value = v;
    }
};


vector<Ident> TID;

vector<string> string_data;

unsigned long put(const string & buf){
    vector<Ident>::iterator k;
 
    if((k = find(TID.begin(), TID.end(), buf)) != TID.end()){
        return k - TID.begin();
    }
    TID.push_back(Ident(buf));
    return TID.size()- 1;
}

class Scanner {
    FILE * fp;
    char c;
    
    /// Search for buf in the list
    int look(const string buf, const char** list){
        int i = 0;
        while(list[i]){
            if(buf == list[i])
                return i;
            ++i;
        }
        return 0;
    }
    
    void get_next_char_from_file(){
        c = fgetc(fp);
    }
    
public:
    static const char* TW [];
    static const char* TD [];
    
    Scanner(const char* program){
        if(!(fp = fopen(program, "r"))){
            throw "can’t open file";
        }
    }
    Lex get_lex();
};
 

 
Lex Scanner::get_lex(){
    /*
     Состояния сканера:
     H - начальное
     IDENT - индетификатор
     NUMB - число
     COM - комментарий
     ALE - присвоить или сравнение
     NEQ - не равно
     COM_OR_DIV - начало комментария или деление
     STRING - строковые данные
     */
    enum state { H, IDENT, NUMB, ALE, NEQ, COM_OR_DIV, COM, STRING};
    int digit = 0;
    unsigned long num_row_table;
    
    ///buffer for identificator
    string buf;
    state current_state = H;
    do {
        get_next_char_from_file();
        switch(current_state){
            case H:
                if(c == ' ' || c == '\n' || c== '\r' || c == '\t'){
                    //do nothing
                }
                else if(isalpha(c)){
                    buf.push_back(c);
                    current_state = IDENT;
                }
                else if(isdigit(c)){
                    digit = c - '0';
                    current_state = NUMB;
                }
                else if(c == '=' || c == '<' || c == '>'){
                    buf.push_back(c);
                    current_state = ALE;
                }
                else if(c == '@'){
                    return Lex(LEX_FIN);
                }
                else if(c == '!'){
                    buf.push_back(c);
                    current_state = NEQ;
                }
                else if(c == '/'){
                    buf.push_back(c);
                    current_state = COM_OR_DIV;
                }
                else if(c == '"'){
                    current_state = STRING;
                }
                else{
                    buf.push_back(c);
                    if((num_row_table = look(buf, TD))){
                        return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table);
                    }
                    else
                        throw c;
                }
                break;
            
            case STRING:
                if(c != '"'){
                    buf.push_back(c);
                }
                else{
                    if(c == '@'){
                        throw c;
                    }
                    else{
                        string_data.push_back(buf);
                        return Lex(LEX_STRING_DATA, string_data.size());
                    }
                }
                break;
                
            case IDENT:
                if(isalpha(c)|| isdigit(c)){
                    buf.push_back(c);
                }
                else {
                    ungetc(c, fp);
                    if((num_row_table = look(buf, TW))){
                        return Lex((type_of_lex)num_row_table, num_row_table);
                    }
                    else {
                        num_row_table = put(buf);
                        return Lex(LEX_ID, num_row_table);
                    }
                }
                break;
                
            case NUMB:
                if(isdigit(c)){
                    digit = digit * 10 + (c - '0');
                }
                else {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, digit);
                }
                break;
                
            case COM_OR_DIV:
                if(c == '*'){
                    current_state = COM;
                }
                else{
                    ungetc(c, fp);
                    if((num_row_table = look(buf, TD))){
                        return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table);
                    }
                    else
                        throw c;
                }
                break;
                
            case COM:
                if(c == '*'){
                    get_next_char_from_file();
                    if(c == '/'){
                        buf = "";
                        current_state = H;
                    }
                }
                else if(c == '@' || c == '{')
                    throw c;
                break;
             
            case ALE:
                if(c == '='){
                    buf.push_back(c);
                    num_row_table = look(buf, TD);
                    return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table);
                }
                else {
                    ungetc(c, fp);
                    num_row_table = look(buf, TD);
                    return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table);
                }
                break;
            case NEQ:
                if(c == '='){
                    buf.push_back(c);
                    num_row_table = look(buf, TD);
                    return Lex(LEX_NEQ, num_row_table);
                }
                else
                    throw '!';
                break;
        } //end switch
    } while(true);
}

const char* Scanner::TW[] = {
    "", "and", "bool", "do", "else",
    "if", "false", "int", "not", "or", "program",
    "read", "true", "while", "write", "string", "struct", NULL
};
 
const char* Scanner::TD[] = {
    "@", "{", "}", ";", ",", "==", "(",
    ")", "=", "<", ">", "+", "-",
    "*", "/", "<=", "!=", ">=", NULL
};
 
ostream & operator<<(ostream &s, Lex l){
    string t;
    if(l.t_lex <= LEX_STRUCT)
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex == LEX_ASSIGN){
        t = "Assign";
    }
    else if(l.t_lex >= LEX_FIN && l.t_lex <= LEX_GEQ)
        t = Scanner::TD[ l.t_lex - LEX_FIN ];
    else if(l.t_lex == LEX_NUM)
        t = "NUMB";
    else if(l.t_lex == LEX_ID)
        t = "ID: " + TID[l.v_lex].get_name();
    else if(l.t_lex == LEX_STRING_DATA)
        t = "STRING_DATA";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}
 
int main(){
    
    auto file_name = "/Users/mikhailvoronin/Documents/Учёба/4 Прога/С++/ModelLangInterpreter/ModelLangInterpreter/Program.txt";
    
    Scanner s(file_name);
    Lex l;
    while ((l = s.get_lex()).get_type() != LEX_FIN){
        cout << l;
    }
    
    for(auto str:string_data){
        cout << str << endl;
    }
    return 0;
}
