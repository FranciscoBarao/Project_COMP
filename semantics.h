#include "structures.h"

basic_type type_to_basic(Type_node type);
int get_number_variables(Structure *node);
void check_second_run(Structure *node, char* scope_name );
void check_program(Structure *node, char* scope_name );
void check_function_invocation(Structure* node ,char* scope_name);
void check_parseArgs(Structure* node ,char* scope_name);
void check_statement(Structure* node ,char* scope_name);
void check_variable(Structure* node ,char* scope_name, char *str, basic_type t);
void check_assign(Structure* node, char* scope_name);
void type_to_node(Structure* node, basic_type type);
void type_error(Structure* node,basic_type t1);
basic_type check_expression(Structure* node, char* scope_name);