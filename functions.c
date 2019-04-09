#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern Scope_element* scope_table;

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

Scope_element* add_scope(char* scope_name, int number_of_params){
    Scope_element *new_scope = (Scope_element*) malloc(sizeof(Scope_element));
    Scope_element *aux;
    Scope_element *previous;

    strcpy(new_scope->name, scope_name);
	new_scope->number_of_params = number_of_params;
	new_scope->next=NULL;
    new_scope->variables=NULL;
    if(scope_table){
        for(aux=scope_table; aux; previous=aux, aux=aux->next){
			if(strcmp(aux->name, scope_name)==0){
                return NULL;
            }
        }
        previous->next=new_scope;	//adiciona ao final da lista
    }else{
        scope_table = new_scope;
    }
}

Scope_element *get_scope(char* scope_name){
    Scope_element *aux;

    for(aux=scope_table; aux; aux=aux->next)
        if(strcmp(aux->name, scope_name)==0)
            return aux;

    return NULL;
}

//Insere um novo identificador na cauda de uma lista ligada de simbolo
Table_element *insert_variable(char* scope_name, char *str, basic_type t){
    Scope_element *scope = get_scope(scope_name);
    if(scope !=NULL){
        Table_element *symbol_table = scope->variables;
        Table_element *new_symbol=(Table_element*) malloc(sizeof(Table_element));
        Table_element *aux;
        Table_element* previous;

        strcpy(new_symbol->name, str);
        new_symbol->type=t;
        new_symbol->next=NULL;	

        if(symbol_table){	
            //Procura cauda da lista e verifica se simbolo ja existe (NOTA: assume-se uma tabela de simbolos globais!)
            for(aux=symbol_table; aux; previous=aux, aux=aux->next)
                if(strcmp(aux->name, str)==0)
                    return NULL;
            
            previous->next=new_symbol;	//adiciona ao final da lista
        }
        else	//symtab tem um elemento -> o novo simbolo
            symbol_table=new_symbol;		
        
        return new_symbol; 
    }
}

void show_table(){
    printf("\n");
    Scope_element *aux_scope;
    Table_element *aux;
    for(aux_scope = scope_table; aux_scope; aux_scope=aux_scope->next){
        printf("Scope %s:\n", aux_scope->name);
        for(aux=aux_scope->variables; aux; aux=aux->next){
            printf("    symbol %s, type %d\n", aux->name, aux->type);
        }
    }
    
}

//Procura um identificador, devolve 0 caso nao exista
Table_element *search_variable(char* scope_name, char *str){
    Scope_element *scope = get_scope(scope_name);
    if(scope != NULL){
        Table_element *symbol_table = scope->variables;
        Table_element *aux;

        for(aux=symbol_table; aux; aux=aux->next){
            if(strcmp(aux->name, str)==0)
                return aux;
        }
    }
    return NULL;
}