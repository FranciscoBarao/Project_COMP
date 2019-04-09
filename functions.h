#include "structures.h"

Structure* create_node(char* val, int col, int l, Type_node type, Structure *child);
void add_brother(Structure *son, Structure *brother);
Scope_element* add_scope(char* scope_name, int number_of_params);
Scope_element *get_scope(char* scope_name);
Table_element *insert_variable(char* scope_name, char *str, basic_type t);
Table_element *search_variable(char* scope_name, char *str);
void show_table();