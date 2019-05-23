#include "structures.h"
#include "semantics.h"
#include "functions.h"


int produce(Structure *node, char* scope_name,int* count_label, int* count, int needs_return);
const char* type_to_llvm(basic_type t );
void produce_header(Structure* node, char* scope_name);
void produce_declarations(char* scope_name);
int produce_statement(Structure* node, char* scope_name, int* count_label,int* count);
void produce_assign(Structure* node, char* scope_name,int* count);
char* produce_expression(Structure* node, char* scope_name, int* count);
void produce_parse_args(Structure *node, char* scope_name, int* count);
void init_produce(Structure *node, int* count_str);
void change_str(Structure* node, Str_meta4 *pointer, int* count_str);
void produce_call(Structure* node, char* scope_name, int* count);
