#include "Parser.h"
#include <iostream>
int main(){
    
    auto file_name = "/Users/mikhailvoronin/Documents/Учёба/4 Прога/С++/ModelLangInterpreter/ModelLangInterpreter/Program.txt";
    
//    Scanner s(file_name);
//    Lex l;
//    while ((l = s.get_lex()).get_type() != LEX_FIN){
//        std::cout << l;
//    }
//    
//    for(auto str:s.string_data){
//        std::cout << str << std::endl;
//    }
//    
//    std::cout << "\nParser: \n\n";
//    
    Parser parser(file_name);
    parser.analyze();
    
    
    // УБРАТЬ PUBLIC!!!
    parser.scan.TID;
    return 0;  
}
