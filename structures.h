#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef enum {
              Program,
              FuncDecl,
              FuncHeader,
              VarDecl,
              FuncParams,
              ParamDecl,
              FuncBody,
              Assign,
              type,
              Block,
              Expression,
              Statement,
              ParseArgs,
              Call,
              id,
              intlit,
              reallit,
              strlit
} Type_node;


typedef enum {integer, string, boolean, float32, function, undef, none} basic_type;

typedef struct _s2{
  char* val;
  int col, l;
}Token;

typedef struct _s1{
  Type_node type;
  basic_type value_type;
  Token *token;
  struct _s1 *brother;
  struct _s1 *child;
}Structure;

typedef struct _t2{
	char name[32];
	basic_type type;
	struct _t2 *next;
} Table_element;

typedef struct _t1{
	char name[32];
  int number_of_params;
  basic_type type;
  struct _t2 *variables;
	struct _t1 *next;
} Scope_element;

#endif