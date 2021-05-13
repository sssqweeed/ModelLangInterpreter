#include "Interpreter.h"
#include <iostream>

int main(){
    
    auto file_name = "/Users/mikhailvoronin/Documents/Учёба/4 Прога/С++/ModelLangInterpreter/ModelLangInterpreter/Program.txt";

//    Parser parser(file_name);
//    parser.analyze();
//
//    parser.print_poliz();

    Interpreter intetpreter(file_name);
    try {
        intetpreter.run();
    } catch (const char* message) {
        std::cout << message;
    }
    
    
//    Scanner scan(file_name);
//    Lex c_lex;
//    while((c_lex = scan.get_lex()).get_type() != LEX_FIN){
//        std::cout << c_lex;
//    }
    return 0;
}
