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