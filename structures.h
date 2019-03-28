#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef enum {
              program,
              declarations,
              function_declaration,
              variable_declaration,
              variable_specification,
              id_list,
              parameters,
              parameters_list,
              type,
              function_body,
              variables_statements,
              statement,
              assign_statement,
              braces_statement,
              statement_list,
              if_statement,
              for_statement,
              return_statement,
              union_statement,
              str_statement,
              parse_arguments,
              function_invocation,
              expression_list,
              expression,
              id,
              intlit,
              reallit,
              strlit,
              no_info,
} Type_node;

typedef struct _s2{
  char* val;
  int col, l;
}Token;

typedef struct _s1{
  Type_node type;
  Token *token;
  struct _s1 *brother;
  struct _s1 *child;
}Structure;

#endif