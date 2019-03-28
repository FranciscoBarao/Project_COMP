#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Structure* create_node(char* val, int col, int l, Type_node type, Structure *child){
    Structure* node = (Structure*)malloc(sizeof(Structure));
    Token* token = (Token*)malloc(sizeof(Token));
    token->val = val;
    token->col = col;
    token->l = l;
    node->brother = NULL;
    node->child = child;
    node->token = token;
    node->type = type;

    return node;
}

void add_brother(Structure *son, Structure *brother){
    if(son != NULL){
        while(son->brother != NULL){
            son = son->brother;
        }
        son->brother = brother;
    }
    //printf("%s\n", brother->token->val);
}