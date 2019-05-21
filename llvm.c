#include "structures.h"
#include "functions.h"
#include "semantics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void check_program(Structure *node, char* scope_name, int* count ){
    //NAO ESQUECER
    //   --> produce_declarations("global");

    if(node == NULL) return;
    Structure* tmp = node;

    switch(tmp->type) {
        case Assign:
            produce_assign(tmp, scope_name,count);
            check_program(tmp->brother, scope_name,count);
            break;
        case FuncDecl:
            produce_header(tmp->child->child, tmp->child->child->token->val);
            produce_declarations(tmp->child->child->token->val);
            check_program(tmp->child, tmp->child->child->token->val,count);
            printf("\n}\n");
            check_program(tmp->brother, scope_name, count);
            break;
        case ParamDecl:
            check_program(tmp->brother, scope_name,count);
            break;
        case Statement:
            produce_statement(node,);
            break;
        default :
            check_program(tmp->child, scope_name,count);
            check_program(tmp->brother, scope_name, count);  
            break;      
    }
}

const char* type_to_llvm(basic_type t ){
    switch(t) {
        case boolean:
            return "i1";
        case float32:
            return "double";
        default:
            return "i32";
    }
}


void produce_header(Structure* node, char* scope_name){

    printf("define %s @%s(",type_to_llvm(node->brother),node->token->val);
    Scope_element* scope = get_scope(scope_name);
    Table_element* aux = NULL;
    if (scope != NULL){
        aux = scope->variables;
        for(int i=0; i<scope->number_of_params;i++){
            printf("%s, ",type_to_llvm(aux->type));
            aux = aux->next;
        }
    }
    printf("\b\b) {\n");

}

void produce_declarations(scope_name){
    //Percorrer tabela simbolos e dar external global ou alloca

    Special_element *special = (Special_element*) malloc(sizeof(Special_element));
    Scope_element *scope = get_scope(scope_name);

    char* tmp = (char*) malloc(sizeof(char)*50);
    if(scope != NULL){
        Table_element *symbol_table = scope->variables;
        Table_element *aux;

        for(aux=symbol_table; aux; aux=aux->next){
            if(strcmp(scope_name, "global")==0)
                if(aux->type != function)
                    printf("@%s = external global %s\n",aux->name,type_to_llvm(aux->type));
            else{
                sprintf(tmp,"%d",scope->number_of_params);
                printf("%%%s = alloca %s\n",aux->name,type_to_llvm(aux->type));
                printf("store %s %%%s, %s* %%%s\n",type_to_llvm(aux->type),tmp,type_to_llvm(aux->type),aux->name);
            }
            i++;
        } 
    }
    return;

}


void produce_statement(Structure* node, char* scope_name, int* count_label){
    if(strcmp(node->token->val, "If")==0 ) {
        //produce_expression(v1)
        //produce_expression(v2)
        //Tem de retornar o contador de temporarios para fazer Tcount para imprimir
        
        //%name = icmp eq i32 %v1, %v2
        //i32? why mesmo?
        //printf("%ifcond%d = icmp i32 %s %s %s",ifcond,node->child->token->val);
        //integer of amount of ifs..? not sure this is the way..
    
        //br i1 %name, label %name2, label %name3
        //printf("br i1 %ifcond%d, label %%s, label %%s",ifcond);
        //ifcond++; 
    }
}

void produce_assign(Structure* node, char* scope_name,int* count){
    char* expr = (char*) malloc(sizeof(char)*50);
    expr = produce_expression(node->child->brother,scope_name,count);
    if(node->is_global)
        printf("store %s %%%s, %s* @%s",type_to_llvm(node),expr,type_to_llvm(node),node->child->token->val);
    else
        printf("store %s %%%s, %s* %%%s",type_to_llvm(node),expr,type_to_llvm(node),node->child->token->val);
    return;
    /*printf("%%%s = ",node->child->token->val);
    if(node->value_type == boolean){
        printf("and i1 %s, true\n",expr);
        return;
    }
    else if(node->value_type ==  float32 ){
        printf("f");
    }
    printf("add %s 0, %s\n", type_to_llvm(node->value_type),expr);
    return;*/

    //store i32 3, i32* %ptr
}

char* produce_expression(Structure* node, char* scope_name, int* count){
    
    char* expr1 = (char*) malloc(sizeof(char)*50);
    char* expr2 = (char*) malloc(sizeof(char)*50);

    char* tmp = (char*) malloc(sizeof(char)*50);


    if(node->type == Block){
        return; //Might not be Correct!
    }else if(node->type == Call){
        
        printf("call %s @%s(",type_to_llvm(node),node->child->token->val);
        
        for(Structure* ptr = node->child->brother;node != NULL;node = node->brother){
            char* expr = produce_expression(ptr, scope_name, count);
            printf("%s %s, ",type_to_llvm(ptr),expr);
        }   
        printf("\b\b)\n");
    }
    
    if(node->child != NULL){
        expr1 = produce_expression(node->child, scope_name,count);
    } 
    if(node->brother != NULL) { 
        expr2 = produce_expression(node->brother, scope_name,count);
    }

    sprintf(tmp,".%d",*count); //var name is %Count
    if(node->type == Expression){
        printf("%%%s = ",tmp);
        if(node->value_type == float32){
            printf("f");
        }

        if(strcmp(node->token->val, "Add")==0){ 
            printf("add %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);

        }else if(strcmp(node->token->val, "Sub")==0){
            printf("sub %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);

        }else if(strcmp(node->token->val, "Mul")==0){
            printf("mul %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);

        }else if(strcmp(node->token->val, "Div")==0){
            printf("sdiv %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);

        }else if(strcmp(node->token->val, "Mod")==0){
            printf("srem %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);   

        }else if(strcmp(node->token->val, "Minus")==0){
            printf("sub %s 0, %s\n",type_to_llvm(node->value_type),expr1);   

        }else if(strcmp(node->token->val, "Plus")==0){
            printf("add %s 0, %s\n",type_to_llvm(node->value_type),expr1);   
        }
        

        else if(strcmp(node->token->val, "Lt")==0){
            printf("icmp slt %s %s %s\n", type_to_llvm(node->value_type), expr1, expr2);

        }else if(strcmp(node->token->val, "Gt")==0){
            printf("icmp sgt %s %s %s\n", type_to_llvm(node->value_type), expr1, expr2);

        }else if(strcmp(node->token->val, "Le")==0){
            printf("icmp sle %s %s %s\n", type_to_llvm(node->value_type), expr1, expr2);

        }else if(strcmp(node->token->val, "Ge")==0){
            printf("icmp sge %s %s %s\n", type_to_llvm(node->value_type), expr1, expr2);

        }else if(strcmp(node->token->val, "Eq")==0){
            printf("icmp eq %s %s %s\n", type_to_llvm(node->value_type), expr1, expr2);

        }else if(strcmp(node->token->val, "Ne")==0){
            printf("icmp ne %s %s %s\n", type_to_llvm(node->value_type), expr1, expr2);

        }
         
        else if(strcmp(node->token->val, "And")==0){
            printf("and i1 %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            
        }else if(strcmp(node->token->val, "Or")==0){
            printf("or i1 %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            
        }else if(strcmp(node->token->val, "Not")==0){
            printf("xor i1 %s, true\n",type_to_llvm(node->value_type),expr1,expr2);
        }

        *count++;
        return tmp;
    }else if(node->type == id){
        printf("%%.%s = load %s, %s* %%%s",tmp,type_to_llvm(node),type_to_llvm(node),node->token->val);
        *count++;
        return tmp;  
    }else{
        //casos primitivos, retorna valor || ex : add var , '2' <--
        return node->token->val;
    }
}


/*
Strlit -> Aloc.. push da string la para dentro e imprime
OU
tabela de ponteiros para por global mais tarde.. <- Mais complexo mas melhor em casos recursivos


*/