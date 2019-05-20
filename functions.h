#include "structures.h"

Structure* create_node(char* val, int col, int l, Type_node type, Structure *child);
void add_brother(Structure *son, Structure *brother);
Scope_element* add_scope(char* scope_name, int number_of_params, basic_type type);
Scope_element *get_scope(char* scope_name);
Table_element *insert_variable(char* scope_name, char *str, basic_type t);
Special_element *search_variable(char* scope_name, char *str);
const char* type_to_string(basic_type t);
const char* expression_to_string(Structure *node);
basic_type val_to_basic(char *val);
basic_type type_to_basic(Type_node type);
void show_table();