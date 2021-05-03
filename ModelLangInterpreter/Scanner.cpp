#include "Scanner.h"

Scanner::Scanner(const char* program){
    if(!(fp = fopen(program, "r"))){
        throw "can’t open file";
    }
    num_of_row_in_file = 0;
}

int Scanner::look(const std::string buf, const char** list){
    int i = 0;
    while(list[i]){
        if(buf == list[i])
            return i;
        ++i;
    }
    return 0;
}

void Scanner::get_next_char_from_file(){
    c = fgetc(fp);
    if (c == '\n') {
        num_of_row_in_file++;
    }
}

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
    std::string buf;
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
                        return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table, num_of_row_in_file);
                    }
                    else
                        throw c;
                }
                break;
            
            case STRING:
                if(c != '"' and c != '@' and c != '\n'){
                    buf.push_back(c);
                }
                else{
                    if(c == '@' or c == '\n'){
                        throw c;
                    }
                    else{
                        string_data.push_back(buf);
                        return Lex(LEX_STRING_DATA, string_data.size(), num_of_row_in_file);
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
                        return Lex((type_of_lex)num_row_table, num_row_table, num_of_row_in_file);
                    }
                    else {
                        num_row_table = put(buf);
                        return Lex(LEX_ID, num_row_table, num_of_row_in_file);
                    }
                }
                break;
                
            case NUMB:
                if(isdigit(c)){
                    digit = digit * 10 + (c - '0');
                }
                else {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, digit, num_of_row_in_file);
                }
                break;
                
            case COM_OR_DIV:
                if(c == '*'){
                    current_state = COM;
                }
                else{
                    ungetc(c, fp);
                    if((num_row_table = look(buf, TD))){
                        return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table, num_of_row_in_file);
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
                    return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table, num_of_row_in_file);
                }
                else {
                    ungetc(c, fp);
                    num_row_table = look(buf, TD);
                    return Lex((type_of_lex)(num_row_table +(int)LEX_FIN), num_row_table, num_of_row_in_file);
                }
                break;
            case NEQ:
                if(c == '='){
                    buf.push_back(c);
                    num_row_table = look(buf, TD);
                    return Lex(LEX_NEQ, num_row_table, num_of_row_in_file);
                }
                else
                    throw '!';
                break;
        } //end switch
    } while(true);
}

unsigned long Scanner::put(const std::string & buf){
    std::vector<Ident>::iterator k;

    if((k = find(TID.begin(), TID.end(), buf)) != TID.end()){
        return k - TID.begin();
    }
    TID.push_back(Ident(buf));
    return TID.size()- 1;
}

const char* Scanner::TW[] = {
    "", "and", "bool", "do", "else",
    "if", "false", "int", "not", "or", "program",
    "read", "true", "while", "for", "write", "string", "struct", "goto", "break", NULL
};

const char* Scanner::TD[] = {
    "@", "{", "}", ";", ",", "==", "(",
    ")", "=", "<", ">", "+", "-",
    "*", "/", "<=", "!=", ">=", ":", NULL
};

std::vector<Ident> Scanner::TID = {};

std::ostream & operator<<(std::ostream &s, Lex l){
    std::string t;
    if(l.t_lex <= LEX_BREAK)
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex == LEX_ASSIGN){
        t = "Assign";
    }
    else if(l.t_lex >= LEX_FIN && l.t_lex <= LEX_COLON)
        t = Scanner::TD[ l.t_lex - LEX_FIN ];
    else if(l.t_lex == LEX_NUM)
        t = "NUMB";
    else if(l.t_lex == LEX_ID)
        t = "ID: " + Scanner::TID[l.v_lex].get_name();
    else if(l.t_lex == LEX_STRING_DATA)
        t = "STRING_DATA";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << "line: " << l.num_row_in_file <<std::endl;
    return s;
}


Scanner::~Scanner(){
    TID.clear();
}
