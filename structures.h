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
  char error[500];
  Token *token;
  Type_node type;
  basic_type value_type;
  int is_global;
  struct _s1 *brother;
  struct _s1 *child;
}Structure;

typedef struct _t2{
	char name[200];
	basic_type type;
  int is_used;
	struct _t2 *next;
} Table_element;

typedef struct _t1{
	char name[200];
  int number_of_params;
  basic_type type;
  struct _t2 *variables;
	struct _t1 *next;
} Scope_element;

typedef struct _t3
{
  struct _t2 *element;
  int is_global;
} Special_element;

typedef struct _s10{
  basic_type type;
  struct _s10 *next;
} Call_types;

#endif