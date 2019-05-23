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
            if(node->error[0] != 0 || node->child->error[0] != 0) return -1;
            is_error += check_second_run(tmp->child, tmp->child->token->val);
            is_error += check_second_run(tmp->brother, tmp->child->token->val);
            break;
        case VarDecl:
            check_variable_for_used(tmp, scope_name);
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
            node->value_type = check_parseArgs(tmp, scope_name);
            is_error += check_second_run(tmp->brother, scope_name);  
            break;
        case Call: //Function Invocation 
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
    Scope_element *scp;
    int is_error = 0;
    switch(tmp->type) {
        case VarDecl:
            is_error += check_variable(tmp->child->brother, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            is_error += check_program(tmp->brother, scope_name);  
            break;
        case FuncHeader:
            scp = add_scope(tmp->child->token->val, get_number_params(tmp), val_to_basic(tmp->child->brother->token->val));
            if(scp == NULL){
                sprintf(node->error, "Line %d, column %d: Symbol %s already defined\n",node->child->token->l,node->child->token->col,node->child->token->val);
                return -1;
            }else{
                is_error += check_variable(tmp->child, scope_name, tmp->child->token->val, function);
                if(is_error < 0) return -1;
                is_error += check_program(tmp->child, tmp->child->token->val);
                is_error += check_program(tmp->brother, tmp->child->token->val);
            }
            break;
        case ParamDecl:
            is_error += check_variable(tmp->child->brother, scope_name, tmp->child->brother->token->val, val_to_basic(tmp->child->token->val));
            Table_element *aux = search_variable(scope_name, tmp->child->brother->token->val)->element;
            aux->is_used = 1;
            is_error += check_program(tmp->brother, scope_name);
            break;
        case id:
            special = search_variable(scope_name, tmp->token->val);
            is_error += check_program(tmp->child, scope_name);
            is_error += check_program(tmp->brother, scope_name); 
            if(special == NULL) break;
            special->element->is_used = 1;
            tmp->is_global = special->is_global;
            break;
        default :
            is_error += check_program(tmp->child, scope_name);
            is_error += check_program(tmp->brother, scope_name);  
            break;      
    }
    return is_error;
}

char* concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int check_function_invocation(Structure* node ,char* scope_name){
    basic_type type_tmp;
    Structure *tmp = node;
    Table_element *variable;
    char *error_string;
    char *error_string2;
    Call_types *linked = malloc(sizeof(Call_types));
    linked->next = NULL;
    Call_types *linked_tmp = linked;
    int number_of_variables = 0;
    int value_to_return = 0;
    Scope_element* scope = get_scope(tmp->child->token->val);
    if(scope != NULL){
        // tmp now is going to be the params of the call
        tmp->value_type = scope->type;
        tmp = tmp->child->brother;
        variable = scope->variables;
        for(int i=0;i<scope->number_of_params; i++){
            if(tmp == NULL) break;
            Call_types *aux = malloc(sizeof(Call_types));
            type_tmp = check_expression_for_call(tmp,scope_name);
            // FALTA AQUI UM CICLO FOR PARA PERCORRER TODAS AS VARIAVEIS E VERIFICAR OS TIPOS
            if(variable->type == type_tmp){
                aux->type = type_tmp;
            }else{
                aux->type = type_tmp;
                value_to_return += -1;
            }
            number_of_variables += 1;
            tmp = tmp->brother;
            variable = variable->next;
            linked_tmp -> next = aux;
            linked_tmp = linked_tmp->next;
        }
        if(number_of_variables != scope->number_of_params || value_to_return < 0){
            linked = linked->next;
            asprintf(&error_string, "Line %d, column %d: Cannot find symbol %s(",node->child->token->l,node->child->token->col,node->child->token->val);
            for(int j=0; j<number_of_variables; j++){
                if(j == number_of_variables-1){
                    asprintf(&error_string2,"%s", type_to_string(linked->type));
                }else{
                    asprintf(&error_string2,"%s,", type_to_string(linked->type));
                }
                error_string = concat(error_string, error_string2);
                free(error_string2);
                linked = linked->next;
            }
            error_string = concat(error_string, ")\n");
            strcpy(node->error, error_string);
            value_to_return += -1;
        }
    }
    else{
        tmp = tmp->child->brother;
        if(tmp != NULL){
            asprintf(&error_string, "Line %d, column %d: Cannot find symbol %s(",node->child->token->l,node->child->token->col,node->child->token->val);
            while(tmp != NULL){
                basic_type var_type = check_expression_for_call(tmp, scope_name);
                if(tmp->brother == NULL){
                    asprintf(&error_string2,"%s", type_to_string(var_type));
                }else{
                    asprintf(&error_string2,"%s,", type_to_string(var_type));
                }
                error_string = concat(error_string, error_string2);
                free(error_string2);
                tmp = tmp->brother;
            }
            error_string = concat(error_string, ")\n");
            strcpy(node->error, error_string);
        }else{
            sprintf(node->error, "Line %d, column %d: Cannot find symbol %s()\n",node->child->token->l,node->child->token->col,node->child->token->val);
        }
        value_to_return += -1;
    }
    value_to_return += check_second_run(node->brother, scope_name);
    return value_to_return; 
}

basic_type check_parseArgs(Structure* node ,char* scope_name){
    Table_element* id;
    Special_element *aux;
    if(node->child->is_global == 1){
        aux = search_variable("global", node->child->token->val);
    }else{
        aux = search_variable(scope_name, node->child->token->val);
    }
    if(aux != NULL){  //variable exists
        id = aux->element;
        id->is_used = 1;
        basic_type temp = check_expression(node->child->brother,scope_name);
        //Int  -> Atoi (args[int])
        if(id->type == integer && temp == integer){
            node->value_type = integer;
            return integer;
        } 
        else{
            sprintf(node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l,node->token->col,expression_to_string(node),type_to_string(id->type),type_to_string(temp));
            return undef;
        }
    }
    else{  //variable doesn´t exist locally or globally
        sprintf(node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
        check_expression(node->child->brother,scope_name);
        return undef;
    }
}

int check_statement(Structure* node ,char* scope_name){
    int value_to_return = 0;
    if(strcmp(node->token->val, "If")==0) {
        basic_type tmp = check_expression(node->child,scope_name);
        if (tmp != boolean){
            sprintf(node->error, "Line %d, column %d: Incompatible type %s in %s statement\n",node->child->token->l,node->child->token->col,type_to_string(tmp),expression_to_string(node));
            value_to_return += -1;
        }
    }else if(strcmp(node->token->val, "For")==0){
        if(strcmp(node->child->token->val, "Block") !=0 ){
            basic_type tmp = check_expression(node->child,scope_name);
            if (tmp != boolean){
                sprintf(node->error, "Line %d, column %d: Incompatible type %s in %s statement\n",node->child->token->l,node->child->token->col,type_to_string(tmp),expression_to_string(node));
                value_to_return += -1;
            }
        }
    }else if(strcmp(node->token->val, "Return") == 0){
        if(node->child ==  NULL){ // no expression and return is none
            //LOOK AT HERE!!
            value_to_return = 0;
        }
        else{
            basic_type tmp = check_expression(node->child,scope_name);
            Scope_element* scope = get_scope(scope_name);
            if(tmp == scope->type);
            else{
                sprintf(node->error, "Line %d, column %d: Incompatible type %s in %s statement\n",node->child->token->l,node->child->token->col,type_to_string(tmp),expression_to_string(node)); 
                value_to_return += -1;
            }
        }
    }
    else{
        check_second_run(node->child, scope_name);
    }
    // Needs this to check code after block
    if(node->child != NULL) value_to_return += check_second_run(node->child->brother, scope_name);
    return value_to_return;
}

int check_variable_use(Structure* node,char* scope_name){
    Table_element* n = search_variable(scope_name,node->token->val)->element;
    if(n != NULL && !n->is_used){
        sprintf(node->error, "Line %d, column %d: Symbol %s declared  but  never  used",node->token->l,node->token->col,node->token->val);
        return -1;
    }
    return 0;
}

void octal(Structure* node, char* octal){
    int i;
    if(octal[0] == '0' && octal[1] != 'x' && octal[1] != 'X'){
        for(i=1;i < sizeof(octal)/sizeof(char); i++){
            if(atoi(&octal[i]) >= 8){
                sprintf(node->error, "Line %d, column %d: Invalid octal constant: %s\n",node->token->l,node->token->col,node->token->val);   
                return;
            }
        }
    }
    return;
}

int check_variable(Structure* node ,char* scope_name, char *str, basic_type t){

    Table_element* new = insert_variable(scope_name, str, t);
    
    if(new == NULL){
        sprintf(node->error, "Line %d, column %d: Symbol %s already defined\n",node->token->l,node->token->col,node->token->val);
        return -1;
    }else{
        node->value_type = t;
        if(strcmp(scope_name, "global") == 0){
            new->is_used = 1;
        }
    }
    return 0;
}

int check_variable_for_used(Structure* node, char* scope_name){
    Structure *aux = node->child->brother;
    Table_element *variable = search_variable(scope_name, aux->token->val)->element;
    if(variable->is_used == 0 && variable->type != function && aux->error[0] == 0){
        sprintf(node->error, "Line %d, column %d: Symbol %s declared but never used\n",aux->token->l,aux->token->col,aux->token->val);
        return -1;
    }else{
        return 0;
    }
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
        id->is_used = 1;
        tmp = check_expression(aux->brother,scope_name);
        node->value_type = tmp;
        if(id->type == tmp){
            //Assign Types are correct
            //[NEEDS DOING] -> meter used_boolean = true;
        }
        else{
            sprintf(node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l, node->token->col, expression_to_string(node), type_to_string(id->type),type_to_string(tmp));
            return -1;
        }
    }
    else{  //variable doesn´t exist locally or globally
        sprintf(aux->error, "Line %d, column %d: Cannot find symbol %s\n",aux->token->l,aux->token->col,aux->token->val);
        tmp = check_expression(aux->brother,scope_name);
        sprintf(node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l, node->token->col, expression_to_string(node), type_to_string(undef),type_to_string(tmp));
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

void type_error(Structure* node, basic_type t1, basic_type t2){
    if(strcmp(node->token->val,"Not") == 0 || strcmp(node->token->val,"Minus") == 0 || strcmp(node->token->val,"Plus") == 0)   sprintf(node->error, "Line %d, column %d: Operator %s cannot be applied to type %s\n",node->token->l,node->token->col,expression_to_string(node),type_to_string(t1));
    else    sprintf(node->error, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token->l,node->token->col,expression_to_string(node),type_to_string(t1),type_to_string(t2));
    return;
}

int check_error_expression(Structure *node, basic_type child_type, basic_type brother_type){
    if(strcmp(node->token->val, "Or") == 0 || strcmp(node->token->val, "And") == 0){
        if(child_type != boolean && brother_type != boolean){
            return -1;
        }else{
            return 0;
        }
    }else if(strcmp(node->token->val, "Add") == 0 || strcmp(node->token->val, "Sub") == 0 || strcmp(node->token->val, "Mul") == 0 || strcmp(node->token->val, "Div") == 0 || strcmp(node->token->val, "Mod") == 0){
        if((child_type == integer && brother_type == integer) || (child_type == float32 && brother_type == float32) || (child_type == string && brother_type == string)){
            return 0;
        }else{
            return -1;
        }
    }else if(strcmp(node->token->val, "Eq") == 0 || strcmp(node->token->val, "Ne") == 0){
        if(child_type == brother_type){
            return 0;
        }else{
            return -1;
        }
    }else{
        if((child_type == integer && brother_type == integer) || (child_type == float32 && brother_type == float32)){
            return 0;
        }else{
            return -1;
        }
    }
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
    }else{
        if(node->child != NULL && node->child->brother != NULL){
            child_type = check_expression(node->child, scope_name);
            brother_type = check_expression(node->child->brother, scope_name);
            if(check_error_expression(node, child_type, brother_type) == -1){
                final_type = undef;
            }else{
                final_type = child_type; //ou brother type e indiferente
            }
        }else if(node->child != NULL){
            child_type = check_expression(node->child, scope_name);
            if(strcmp(node->token->val, "Plus") == 0 || strcmp(node->token->val, "Minus") == 0){
                if(child_type == integer || child_type == float32){
                    final_type = child_type;
                }else{
                    final_type = undef;
                }
            }else if(strcmp(node->token->val, "Not") == 0){
                if(child_type == boolean){
                    final_type = child_type;
                }else{
                    final_type = undef;
                }
            }else{
                final_type = undef;
            }
        }else{
            switch(node->type){
                case intlit:
                    octal(node, node->token->val);
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
                    if (aux != NULL && aux->element->type != function) {  //Existe
                        tmp = aux->element;
                        tmp->is_used = 1;
                        final_type = tmp->type;
                    }else{
                        final_type = undef;
                        sprintf(node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
                    }
                    break;
                default:
                    final_type = undef;
                    break;
            }
        }
    }
    if(final_type == undef && node->type == Expression ) type_error(node,child_type,brother_type);
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
    basic_type child_type;
    basic_type final_type;
    basic_type brother_expression_type;
    Table_element *tmp;
    switch(node->type){
        case intlit:
            octal(node, node->token->val);
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
            if (aux != NULL && aux->element->type != function) {  //Existe
                tmp = aux->element;
                tmp->is_used = 1;
                final_type = tmp->type;
            }else{
                final_type = undef;
                sprintf(node->error, "Line %d, column %d: Cannot find symbol %s\n",node->token->l,node->token->col,node->token->val);
            }
            break;
        case Expression:
            child_type = check_expression_for_call(node->child, scope_name);
            if(node->child->brother != NULL){
                brother_expression_type = check_expression_for_call(node->child->brother, scope_name);
                if(check_error_expression(node, child_type, brother_expression_type) == -1){
                    final_type = undef;
                }else{
                    final_type = child_type; //ou brother type e indiferente
                }
            }else{
                if(strcmp(node->token->val, "Plus") == 0 || strcmp(node->token->val, "Minus") == 0){
                    if(child_type == integer || child_type == float32){
                        final_type = child_type;
                    }else{
                        final_type = undef;
                    }
                }else if(strcmp(node->token->val, "Not") == 0){
                    if(child_type == boolean){
                        final_type = child_type;
                    }else{
                        final_type = undef;
                    }
                }else{
                    final_type = undef;
                }
            }
            break;
        default:
            final_type = none;
            break;
    }
    if(final_type == undef && node->type == Expression ) type_error(node,child_type,brother_expression_type);
    final_type = type_to_node(node,final_type);
    return final_type;
}
