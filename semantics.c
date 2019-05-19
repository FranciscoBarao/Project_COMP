#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
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

int check_second_run(Structure *node, char* scope_name ){ //Second run of tree
    if(node == NULL) return 0;
    Structure* tmp = node;
    int is_error = 0;
    switch(tmp->type) {
        case FuncHeader:
            is_error += check_second_run(tmp->child, tmp->child->token->val);
            is_error += check_second_run(tmp->brother, tmp->child->token->val);
            break;
        case VarDecl:
            //Function to check Variable "used_boolean".. if false -> error -> declared but not used
            //check_variable(tmp, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            is_error += check_second_run(tmp->brother, scope_name);
            break;
        case Assign:
            is_error += check_assign(tmp, scope_name);
            is_error += check_second_run(tmp->brother, scope_name);
            break;
        case Statement:
            is_error += check_statement(tmp, scope_name);
            is_error += check_second_run(tmp->brother, scope_name);  
            break;
        case ParseArgs:
            node->value_type = check_parseArgs(tmp->child, scope_name);
            is_error += check_second_run(tmp->brother, scope_name);  
            break;
        case Call: //Function Invocation 
            //Check if they exist and if params are well
            is_error += check_function_invocation(tmp, scope_name);
            is_error += check_second_run(tmp->brother, scope_name);
            break;
        default:
            is_error += check_second_run(tmp->child, scope_name);
            is_error += check_second_run(tmp->brother, scope_name);  
            break;      
    }
    return is_error;
}

int check_program(Structure *node, char* scope_name ){ //First run of tree
    if(node == NULL) return 0;
    Structure* tmp = node;
    Special_element *special;
    int is_error = 0;

    switch(tmp->type) {
        case VarDecl:
            is_error += check_variable(tmp->child->brother, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            is_error += check_program(tmp->brother, scope_name);  
            break;
        case FuncHeader:
            add_scope(tmp->child->token->val, get_number_params(tmp), val_to_basic(tmp->child->brother->token->val));
            is_error += check_variable(tmp->child, scope_name, tmp->child->token->val, function);
            is_error += check_program(tmp->child, tmp->child->token->val);
            is_error += check_program(tmp->brother, tmp->child->token->val);
            break;
        case ParamDecl:
            is_error += check_variable(tmp->child->brother, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            is_error += check_program(tmp->brother, scope_name);
            break;
        case id:
            special = search_variable(scope_name, tmp->token->val);
            is_error += check_program(tmp->child, scope_name);
            is_error += check_program(tmp->brother, scope_name); 
            if(special == NULL) break;
            tmp->is_global = special->is_global;
            break;
        default :
            is_error += check_program(tmp->child, scope_name);
            is_error += check_program(tmp->brother, scope_name);  
            break;      
    }
    return is_error;
}

int check_function_invocation(Structure* node ,char* scope_name){
    basic_type type_tmp;
    Structure *tmp = node;
    Table_element *variable;
    int number_of_variables = 0;
    tmp->value_type = get_scope(tmp->child->token->val)->type;
    tmp = tmp->child;
    // tmp is now id of call 
    Scope_element* scope = get_scope(tmp->token->val); // Bem?
    // tmp now is going to be the params of the call
    tmp = tmp->brother;
    variable = scope->variables;
    if(scope != NULL){
        for(int i=0;i<scope->number_of_params; i++){
            if(tmp == NULL) break;
            type_tmp = check_expression_for_call(tmp,scope_name);
            // FALTA AQUI UM CICLO FOR PARA PERCORRER TODAS AS VARIAVEIS E VERIFICAR OS TIPOS
            if(variable->type == type_tmp){

            } // Parameter type esta de acordo com a expressao na invocacao
            else{
                //VER ISTO.. WTF?!
                //De acordo com exemplos é assim mas nao faz sentido?
                asprintf(&tmp->error, "Line %d, column %d: Cannot find symbol %s\n",tmp->token->l,tmp->token->col,tmp->token->val);
                return -1;
            }
            number_of_variables += 1;
            tmp = tmp->brother;
            variable = variable->next;
        }
        if(number_of_variables != scope->number_of_params){
            asprintf(&node->child->error, "Line %d, column %d: Cannot find symbol %s()\n", node->child->token->l,node->child->token->col,node->child->token->val);
        }
    }
    else{
        asprintf(&tmp->error, "Line %d, column %d: Cannot find symbol %s()\n",tmp->token->l,tmp->token->col,tmp->token->val);
        return -1;
    }
    return check_second_run(node->brother, scope_name);
}

basic_type check_parseArgs(Structure* node ,char* scope_name){
    Table_element* id;
    Special_element *aux;
    if(node->is_global == 1){
        aux = search_variable("global", node->token->val);
    }else{
        aux = search_variable(scope_name, node->token->val);
    }
    if(aux != NULL){  //variable exists
        id = aux->element;
        basic_type temp = check_expression(node->brother,scope_name);
        //Int  -> Atoi (args[int])
        if(id->type == integer && temp == integer){
            return integer;
        } //Assign Types are correct
        //[NEEDS DOING] --> Meter boolean a true de ser usado  
        else{
            asprintf(&node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l,node->token->col,expression_to_string(node),type_to_string(id->type),type_to_string(temp));
            return undef;
        }
    }
    else{  //variable doesn´t exist locally or globally
        asprintf(&node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
        return undef;
    }
}

int check_statement(Structure* node ,char* scope_name){
    if(strcmp(node->token->val, "If")==0 || strcmp(node->token->val, "For")==0) {
        basic_type tmp = check_expression(node->child,scope_name);
        if (tmp != boolean){
            asprintf(&node->error, "Line %d, column %d: Incompatible type %s in %s statement\n",node->token->l,node->token->col,type_to_string(tmp),node->token->val);
            return -1;
        }
    }
    else if(strcmp(node->token->val, "Return") == 0){
        if(node->child ==  NULL){ // no expression and return is none
        //LOOK AT HERE!!
            return 0;
        }
        else{
            basic_type tmp = check_expression(node->child,scope_name);
            Scope_element* scope = get_scope(scope_name);
            if(tmp == scope->type);
            else{
                asprintf(&node->error, "Line %d, column %d: Incompatible type %s in %s statement",node->token->l,node->token->col,type_to_string(tmp),node->token->val); 
                return -1;
            }
        }
    }
    else{
        check_second_run(node->child, scope_name);
    }
    // Needs this to check code after block
    return check_second_run(node->child->brother, scope_name);
}

int check_variable_use(Structure* node,char* scope_name){
    Table_element* n = search_variable(scope_name,node->token->val)->element;
    if(n != NULL && !n->is_used){
        asprintf(&node->error, "Line %d, column %d: Symbol %s declared  but  never  used",node->token->l,node->token->col,node->token->val);   
        return -1;
    }
    return 0;
}

int octal(Structure*node, char* octal){
    int i;
    if(atoi(&octal[0]) == 0 && octal[1] != 'x' && octal[1] != 'X'){
        for(i=1;i < sizeof(octal)/sizeof(char); i++){
            if(atoi(&octal[i]) >= 8){
                asprintf(&node->error, "Line %d, column %d: Invalid  octal  constant: %s",node->token->l,node->token->col,node->token->val);   
                return -1;
            }
        }
    }
    return 0;
}

int check_variable(Structure* node ,char* scope_name, char *str, basic_type t){

    Table_element* new = insert_variable(scope_name, str, t);
    
    if(new == NULL){
        asprintf(&node->error, "Line %d, column %d: Symbol %s already defined\n",node->token->l,node->token->col,node->token->val);
        return -1;
    }
    return 0;
}

int check_assign(Structure* node, char* scope_name){
    Structure *aux = node;
    Special_element *special;
    basic_type tmp;
    Table_element* id;
    aux = aux->child;
    if(aux->is_global == 1){
        special = search_variable("global", aux->token->val);
    }else{
        special = search_variable(scope_name, aux->token->val);
    }
    if(special != NULL){  //variable exists
        id = special->element;
        tmp = check_expression(aux->brother,scope_name);
        node->value_type = tmp;
        if(id->type == tmp){
            //Assign Types are correct
            //[NEEDS DOING] -> meter used_boolean = true;
        }
        else{
            asprintf(&node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l, node->token->col, expression_to_string(node), type_to_string(id->type),type_to_string(tmp));
            return -1;
        }
    }
    else{  //variable doesn´t exist locally or globally
        asprintf(&aux->error, "Line %d, column %d: Cannot find symbol %s\n",aux->token->l,aux->token->col,aux->token->val);
        return -1;
    }
    return 0;
}

basic_type type_to_node(Structure* node, basic_type type){
    char boolean_options[9][4] ={"Or","And","Lt","Gt","Le","Ge","Eq","Ne","Not"};
    for(int i=0;i<9;i++){
        if(strcmp(node->token->val,boolean_options[i])==0){
            node->value_type = boolean;
            return boolean;
        }
    }
    node->value_type = type;
    return type;
}

void type_error(Structure* node,basic_type t1){
    if(strcmp(node->token->val,"Not") == 0)   asprintf(&node->error, "Line %d, column %d: Operator %s cannot be applied to type %s\n",node->token->l,node->token->col,expression_to_string(node),type_to_string(node->value_type));
    else    asprintf(&node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l,node->token->col,expression_to_string(node),type_to_string(node->value_type),type_to_string(t1));
    return;
}

basic_type check_expression(Structure* node, char* scope_name){
    Table_element* tmp;
    Special_element *aux;
    basic_type child_type;
    basic_type brother_type;
    basic_type final_type;
    if(node->type == Block){
        return none;
    }else if(node->type == Call){
        check_function_invocation(node, scope_name);
        return node->value_type;
    }
    if(node->child != NULL){
        child_type = check_expression(node->child, scope_name);
    } 
    if(node->brother != NULL && node->brother->type != Block) {
        brother_type = check_expression(node->brother, scope_name);
    }

    if(node->brother != NULL && node->child != NULL && node->brother->type != Block){
        if(child_type != brother_type){
            if(node->brother->type == Block){
                final_type = child_type;
            }else{
                final_type = undef;
                // Parte de errovem para aqui
            }
        }else{
            final_type = child_type; // Tem de ser o child
        }
    }else if(node->brother != NULL && node->brother->type != Block){
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
                if(node->is_global == 1){
                    aux = search_variable("global", node->token->val);
                }else{
                    aux = search_variable(scope_name, node->token->val);
                }
                if (aux != NULL) {  //Existe
                    tmp = aux->element;
                    final_type = tmp->type == brother_type ? brother_type : none;
                }else{
                    final_type = undef;
                    asprintf(&node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
                }
                break;
            default:
                final_type = undef;
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
                if(node->is_global == 1){
                    aux = search_variable("global", node->token->val);
                }else{
                    aux = search_variable(scope_name, node->token->val);
                }
                if (aux != NULL) {  //Existe
                    tmp = aux->element;
                    final_type = tmp->type;
                }else{
                    final_type = undef;
                    asprintf(&node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
                }
                break;
            default:
                final_type = none;
                break;
        }
    }
    if(final_type == undef) type_error(node,brother_type);
    final_type = type_to_node(node,final_type);
    return final_type;
}

basic_type check_expression_for_call(Structure *node, char* scope_name){
    if(node == NULL) return undef;

    if(node->type == Call){
        check_function_invocation(node, scope_name);
        return node->value_type;
    }
    Special_element *aux;
    basic_type final_type;
    basic_type brother_expression_type;
    Table_element *tmp;
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
            if(node->is_global == 1){
                aux = search_variable("global", node->token->val);
            }else{
                aux = search_variable(scope_name, node->token->val);
            }
            if (aux != NULL) {  //Existe
                tmp = aux->element;
                final_type = tmp->type;
            }else{
                final_type = undef;
                asprintf(&node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
            }
            break;
        case Expression:
            final_type = check_expression_for_call(node->child, scope_name);
            if(node->child->brother != NULL){
                brother_expression_type = check_expression_for_call(node->child->brother, scope_name);
                final_type = brother_expression_type == final_type ? final_type : undef;
            }
            final_type = type_to_node(node,final_type);
            break;
        default:
            final_type = none;
            break;
    }
    return final_type;
}
