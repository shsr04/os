#include "term.hpp"

extern "C" void mod_print() { term::write("Print module loaded."); }
