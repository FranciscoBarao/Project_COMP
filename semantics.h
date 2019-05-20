#include "structures.h"

basic_type type_to_basic(Type_node type);
int get_number_variables(Structure *node);
int check_second_run(Structure *node, char* scope_name );
int check_program(Structure *node, char* scope_name );
int check_function_invocation(Structure* node ,char* scope_name);
basic_type check_parseArgs(Structure* node ,char* scope_name);
int check_statement(Structure* node ,char* scope_name);
int check_variable(Structure* node ,char* scope_name, char *str, basic_type t);
int check_variable_for_used(Structure* node, char* scope_name);
int check_assign(Structure* node, char* scope_name);
basic_type type_to_node(Structure* node, basic_type type);
void type_error(Structure* node, basic_type t1, basic_type t2);
basic_type check_expression(Structure* node, char* scope_name);
basic_type check_expression_for_call(Structure *node, char* scope_name);