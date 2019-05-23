#include "structures.h"
#include "semantics.h"
#include "functions.h"


int produce(Structure *node, char* scope_name,int* count_label, int* count, int* count_str, int needs_return, Str_meta4* pointer);
const char* type_to_llvm(basic_type t );
void produce_header(Structure* node, char* scope_name);
void produce_declarations(char* scope_name);
int produce_statement(Structure* node, char* scope_name, int* count_label,int* count, int* count_str, Str_meta4* pointer);
void produce_assign(Structure* node, char* scope_name,int* count);
char* produce_expression(Structure* node, char* scope_name, int* count);
void produce_parse_args(Structure *node, char* scope_name, int* count);
void init_produce();
int change_str(Structure* node, char* scope_name, Str_meta4* pointer, int* count_str);

