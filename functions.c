#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern Scope_element* scope_table;

const char* expression_to_string(Structure *node){
    if(strcmp(node->token->val, "Assign") == 0){
        return "=";
    }else if(strcmp(node->token->val, "Or") == 0){
        return "||";
    }else if(strcmp(node->token->val, "And") == 0){
        return "&&";
    }else if(strcmp(node->token->val, "Lt") == 0){
        return "<";
    }else if(strcmp(node->token->val, "Gt") == 0){
        return ">";
    }else if(strcmp(node->token->val, "Le") == 0){
        return "<=";
    }else if(strcmp(node->token->val, "Ge") == 0){
        return ">=";
    }else if(strcmp(node->token->val, "Eq") == 0){
        return "==";
    }else if(strcmp(node->token->val, "Ne") == 0){
        return "!=";
    }else if(strcmp(node->token->val, "Add") == 0){
        return "+";
    }else if(strcmp(node->token->val, "Sub") == 0){
        return "-";
    }else if(strcmp(node->token->val, "Mul") == 0){
        return "*";
    }else if(strcmp(node->token->val, "Div") == 0){
        return "/";
    }else if(strcmp(node->token->val, "Not") == 0){
        return "!";
    }else if(strcmp(node->token->val, "If") == 0){
        return "if";
    }else if(strcmp(node->token->val, "For") == 0){
        return "for";
    }else if(strcmp(node->token->val, "ParseArgs") == 0){
        return "strconv.Atoi";
    }else{
        return node->token->val;
    }
}

const char* type_to_string(basic_type t){
    switch(t) {
        case integer:
            return "int";
        case string:
            return "string";
        case boolean:
            return "bool";
        case float32:
            return "float32";
        case none:
            return "none";
        case undef:
            return "undef";
        case function:
            return "teste";
        default:
            return "error";
    }
}

basic_type type_to_basic(Type_node type){
    switch (type){
    case reallit:
        return float32;
    case intlit:
        return integer;
    case strlit:
        return string;
    default:
        return none;
    }
}

basic_type val_to_basic(char *val){
    if(strcmp(val, "Int")==0){
        return integer;
    }else if(strcmp(val, "Float32")==0){
        return float32;
    }else if(strcmp(val, "String")==0){
        return string;
    }else if(strcmp(val, "Bool")==0){
        return boolean;
    }else{
        return none;
    }
}

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
    node->value_type = undef;
    node->is_global = 0;

    return node;
}

void add_brother(Structure *son, Structure *brother){
    if(son != NULL){
        while(son->brother != NULL){
            son = son->brother;
        }
        son->brother = brother;
    }
}

Scope_element* add_scope(char* scope_name, int number_of_params, basic_type type){
    Scope_element *new_scope = (Scope_element*) malloc(sizeof(Scope_element));
    Scope_element *aux;
    Scope_element *previous;

    strcpy(new_scope->name, scope_name);
	new_scope->number_of_params = number_of_params;
	new_scope->next=NULL;
    new_scope->variables=NULL;
    new_scope->type = type;
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
    return new_scope;
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
    if(scope != NULL){
        Table_element *symbol_table = scope->variables;
        Table_element *new_symbol=(Table_element*) malloc(sizeof(Table_element));
        Table_element *aux;
        Table_element* previous;

        strcpy(new_symbol->name, str);
        new_symbol->type=t;
        new_symbol->next=NULL;
        new_symbol->is_used=0;
        if(symbol_table){	
            //Procura cauda da lista e verifica se simbolo ja existe
            for(aux=symbol_table; aux; previous=aux, aux=aux->next)
                if(strcmp(aux->name, str)==0)
                    return NULL;
            
            previous->next=new_symbol;	//adiciona ao final da lista
        }
        else	//symtab tem um elemento -> o novo simbolo
            scope->variables = new_symbol;		
        
        return new_symbol; 
    }
    return NULL;
}

void show_table(){
    Scope_element *aux_scope;
    Scope_element *scope;
    Table_element *aux;
    Table_element *param;
    int inside_params;
    for(aux_scope = scope_table; aux_scope; aux_scope=aux_scope->next){
        inside_params = 0; // Variable used to check if variable is still a param of the function
        if(strcmp(aux_scope->name, "global") == 0){
            printf("===== Global Symbol Table =====\n");
        }else{
            printf("===== Function %s(", aux_scope->name);
            // percorre as variaveis e imprime os tipos delas
            scope = get_scope(aux_scope->name);
            if(scope != NULL){
                param = scope->variables;
                if(param != NULL){
                    for(int i=0; i<scope->number_of_params; i++){
                        printf("%s", type_to_string(param->type));
                        if(i < scope->number_of_params-1){
                            printf(",");
                        }
                        param = param->next;
                    }
                }
            }
            printf(") Symbol Table =====\n");
            printf("return\t\t%s\n", type_to_string(aux_scope->type));
        }
        for(aux=aux_scope->variables; aux; aux=aux->next){
            printf("%s\t", aux->name); //%s)\t%s\t%s\n", aux->name, aux->name, aux->name, aux->name);
            if(aux->type == function){
                printf("(");
                // percorre as variaveis e imprime os tipos delas
                scope = get_scope(aux->name);
                if(scope != NULL){
                    param = scope->variables;
                    if(param != NULL){
                        for(int i=0; i<scope->number_of_params; i++){
                            printf("%s", type_to_string(param->type));
                            if(i < scope->number_of_params-1){
                                printf(",");
                            }
                            param = param->next;
                        }
                    }
                }
                printf(")\t%s", type_to_string(scope->type));
            }else{
                if(inside_params < aux_scope->number_of_params){
                    printf("\t%s\tparam", type_to_string(aux->type));
                    inside_params += 1;
                }else{
                    printf("\t%s", type_to_string(aux->type));
                }
            }
            printf("\n");
        }
        printf("\n");
    }
}

//Procura um identificador, devolve 0 caso nao exista
Special_element *search_variable(char* scope_name, char *str){
    Special_element *special = (Special_element*) malloc(sizeof(Special_element));
    Scope_element *scope = get_scope(scope_name);
    if(scope != NULL){
        Table_element *symbol_table = scope->variables;
        Table_element *aux;

        for(aux=symbol_table; aux; aux=aux->next){
            if(strcmp(aux->name, str)==0){
                special->element = aux;
                if(strcmp(scope_name, "global") == 0){
                    special->is_global = 1;
                }else{
                    special->is_global = 0;
                }
                return special;
            }
        }
    }
    if(strcmp(scope_name, "global") != 0){
        return search_variable("global", str);
    }
    return NULL;
}