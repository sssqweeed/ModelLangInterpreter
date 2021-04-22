#include "Lex.h"

Lex::Lex(type_of_lex t, unsigned long v, long _num_row_in_file): t_lex(t), v_lex(v),
                                                                num_row_in_file(_num_row_in_file){  }

type_of_lex Lex::get_type() const {
    return t_lex;
}

unsigned long Lex::get_value() const {
    return v_lex;
}

