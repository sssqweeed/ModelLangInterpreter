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
        
    }
    return 0;
}
