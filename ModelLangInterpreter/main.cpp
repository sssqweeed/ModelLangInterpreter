#include "Parser.h"
#include <iostream>

int main(){
    
    auto file_name = "/Users/mikhailvoronin/Documents/Учёба/4 Прога/С++/ModelLangInterpreter/ModelLangInterpreter/Program.txt";
    
    Parser parser(file_name);
    parser.analyze();

    parser.print_poliz();
    return 0;
}
