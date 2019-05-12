#include "structures.h"
#include "functions.h"
#include "semantics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_number_params(Structure *node){
    Structure *tmp = node->child;
    int number_params = 0;
    while(tmp->type != FuncParams){
        tmp = tmp->brother;
    }
    tmp = tmp->child;
    if(tmp == NULL) return 0;
    while(tmp != NULL && tmp->type == ParamDecl){
        tmp = tmp->brother;
        number_params += 1;
    }
    return number_params;
}

void check_second_run(Structure *node, char* scope_name ){ //Second run of tree
    if(node == NULL) return;
    Structure* tmp = node;
    switch(tmp->type) {
        case VarDecl:
            //Function to check Variable "used_boolean".. if false -> error -> declared but not used
            //check_variable(tmp, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            check_second_run(tmp->brother, scope_name);
            break;
        case Call: //Function Invocation 
            //Check if they exist and if params are well
            tmp->value_type = get_scope(tmp->child->token->val)->type;
            //check_function_invocation(tmp->child, scope_name);
            check_second_run(tmp->child, scope_name);
            check_second_run(tmp->brother, scope_name);
            break;
        default:
            check_second_run(tmp->child, scope_name);
            check_second_run(tmp->brother, scope_name);  
            break;      
    }
}

void check_program(Structure *node, char* scope_name ){ //First run of tree
    if(node == NULL) return;
    Structure* tmp = node;

    switch(tmp->type) {
        case VarDecl:
            check_variable(tmp, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            check_program(tmp->brother, scope_name);  
            break;
        case Assign:
            check_assign(tmp, scope_name); 
            check_program(tmp->brother, scope_name);  
            break;
        case FuncHeader:
            add_scope(tmp->child->token->val, get_number_params(tmp), val_to_basic(tmp->child->brother->token->val));
            check_variable(tmp, scope_name, tmp->child->token->val, function);
            check_program(tmp->child, tmp->child->token->val);
            check_program(tmp->brother, tmp->child->token->val);
            break;
        case Statement:
            check_statement(tmp, scope_name);
            check_program(tmp->brother, scope_name);  
            break;
        case ParseArgs:
            check_parseArgs(tmp->child, scope_name);
            check_program(tmp->brother, scope_name);  
            break;
        case ParamDecl:
            check_variable(tmp, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            check_program(tmp->brother, scope_name);
        default :
            check_program(tmp->child, scope_name);
            check_program(tmp->brother, scope_name);  
            break;      
    }
}

void check_function_invocation(Structure* node ,char* scope_name){
    Scope_element* scope = get_scope(node->token->val); // Bem?
    if(scope != NULL){
        for(int i=0;i<scope->number_of_params; i++){
            basic_type tmp = check_expression(node->child,scope_name);
            // FALTA AQUI UM CICLO FOR PARA PERCORRER TODAS AS VARIAVEIS E VERIFICAR OS TIPOS
            if(scope->variables->type == tmp){

            } // Parameter type esta de acordo com a expressao na invocacao
            else{
                //VER ISTO.. WTF?!
                //De acordo com exemplos é assim mas nao faz sentido?
                sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
            }  
        }
    }
    else{
        sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
    }   
    

}

void check_parseArgs(Structure* node ,char* scope_name){
    
    Table_element* id =  search_variable(scope_name, node->token->val);
    if(id != NULL){  //variable exists
        basic_type temp = check_expression(node->brother,scope_name);
        //Int  -> Atoi (args[int])
        if(id->type == integer && temp == integer){

        } //Assign Types are correct
        //[NEEDS DOING] --> Meter boolean a true de ser usado  
        else{
            sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to types %s, %s",node->token->l,node->token->col,node->token->val,type_to_string(id->type),type_to_string(temp));
        }
    }
    else{  //variable doesn´t exist locally or globally
        sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
    }
}

void check_statement(Structure* node ,char* scope_name){

    if(strcmp(node->token->val, "If")==0 || strcmp(node->token->val, "For")==0) {
        basic_type tmp = check_expression(node->child,scope_name);
        if (tmp != boolean){
            sprintf(node->error,"Line %d, column %d: Incompatible type %s in %s statement",node->token->l,node->token->col,type_to_string(tmp),node->token->val);
        }
    }
    else if(strcmp(node->token->val, "Return") == 0){
        if(node->child ==  NULL){ // no expression and return is none
        //LOOK AT HERE!!
        }
        else{
            basic_type tmp = check_expression(node->child,scope_name);
            Scope_element* scope = get_scope(scope_name);
            if(tmp == scope->type);
            else{
                sprintf(node->error,"Line %d, column %d: Incompatible type %s in %s statement",node->token->l,node->token->col,type_to_string(tmp),node->token->val); 
            }
        }
    }
    else{
        check_program(node->child, scope_name);
    }
}

void check_variable(Structure* node ,char* scope_name, char *str, basic_type t){

    Table_element* new = insert_variable(scope_name, str, t);
    
    if(new == NULL){
        sprintf(node->error,"Line %d, column %d: Symbol %s already defined",node->token->l,node->token->col,node->token->val);
    }
    return;
}

void check_assign(Structure* node, char* scope_name){
    Structure *aux = node;
    aux = aux->child;
    Table_element* id =  search_variable(scope_name, aux->token->val);
    if(id != NULL){  //variable exists
        basic_type tmp = check_expression(aux->brother,scope_name);
        node->value_type = tmp;
        if(id->type == tmp); //Assign Types are correct
        //[NEEDS DOING] -> meter used_boolean = true;
        else{
            sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to types %s, %s",node->token->l,node->token->col,node->token->val,type_to_string(id->type),type_to_string(tmp));
        }
    }
    else{  //variable doesn´t exist locally or globally
        sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
    }
    return;
}

void type_to_node(Structure* node, basic_type type){
    char boolean_options[9][4] ={"Or","And","Lt","Gt","Le","Ge","Eq","Ne","Not"};
    for(int i=0;i<9;i++){
        if(strcmp(node->token->val,boolean_options[i])==0){
            node->value_type = boolean;
        }
    }
    node->value_type = type;
    return;
}

void type_error(Structure* node,basic_type t1){
    if(strcmp(node->token->val,"Not") == 0)   sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to type %s",node->token->l,node->token->col,node->token->val,type_to_string(node->value_type));
    else    sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to types %s, %s",node->token->l,node->token->col,node->token->val,type_to_string(node->value_type),type_to_string(t1));
    return;
}

basic_type check_expression(Structure* node, char* scope_name){
    if(node->type == Block){
        check_program(node, scope_name);
        return none;
    }
    Table_element* tmp;
    basic_type child_type;
    basic_type brother_type;
    basic_type final_type;
    if(node->child != NULL){
        child_type = check_expression(node->child, scope_name);
    } 
    else if(node->brother != NULL) {
        brother_type = check_expression(node->brother, scope_name);
    }

    if(node->brother != NULL && node->child != NULL){
        if(child_type != brother_type){
        // Fazer a parte do erro caso sejam diferentes
        }else{
            final_type = child_type; // Aqui é indiferente qual deles é
        }
    }else if(node->brother != NULL){
        switch(node->type){
        case intlit:
                final_type = integer == brother_type ? integer : undef;
                break;
            case reallit:
                final_type = float32 == brother_type ? float32 : undef;
                break;
            case strlit:
                final_type = string == brother_type ? string : undef;
                break;
            case id:
                tmp =  search_variable(scope_name, node->token->val);
                if (tmp != NULL) {  //Existe
                    final_type = tmp->type == brother_type ? brother_type : undef;    
                }else{
                    final_type = undef;
                    sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,tmp->name);
                }
                break;
            default:
                final_type = none;
                break;
        }
    }else if(node->child != NULL){
        final_type = child_type;
    }else{
        switch(node->type){
        case intlit:
                final_type = integer;
                break;
            case reallit:
                final_type = float32;
                break;
            case strlit:
                final_type = string;
                break;
            case id:
                tmp =  search_variable(scope_name, node->token->val);
                if (tmp != NULL) {  //Existe
                    final_type = tmp->type;         
                }else{
                    final_type = undef;
                    sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,tmp->name);
                }
                break;
            default:
                final_type = none;
                break;
        }
    }
     //if(final_type == undef) type_error(node,temp);
    type_to_node(node,final_type);
    return final_type;
}
