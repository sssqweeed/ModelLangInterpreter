#include "Lex.h"

Lex::Lex(type_of_lex t, unsigned long v): t_lex(t), v_lex(v) {  }

type_of_lex Lex::get_type() const {
    return t_lex;
}

unsigned long Lex::get_value() const {
    return v_lex;
}

