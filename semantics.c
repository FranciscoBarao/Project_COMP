#include "structures.h"
#include "semantics.h"
#include "symbol_table.h"
#include <stdio.h>

void check_program_run2(Structure *node, char* scope_name ){ //Second run of tree
    Structure* tmp = node;
    if(tmp != NULL){
        switch(tmp->type) {
            case VarDecl:
                //Function to check Variable "used_boolean".. if false -> error -> declared but not used
                check_variable_usage(tmp->child, scope_name);
                break;
            case Call: //Function Invocation 
                //Check if they exist and if params are well
                check_function_invocation(node->child, scope_name);
                break;
            default :
                check_program(tmp->child, scope_name);
                check_program(tmp->brother, scope_name);  
                break;      
        }
    }
}

void check_program(Structure *node, char* scope_name ){ //First run of tree
    Structure* tmp = node;
    
    if(tmp != NULL){
        switch(tmp->type) {
            case VarDecl:
                check_variable(tmp->child, scope_name);
                break;
            case Assign:
                check_assign(tmp->child, scope_name); 
                break;
            case FuncHeader:
                //[FALTA ISTO]
                //AddScope(Node->token->val, number_parameters) --> while node->child->brother == ParamDecl count++
                check_program(tmp->child, tmp->token->val);
                check_program(tmp->brother, tmp->token->val);
                break;
            case Statement:
                check_statement(tmp->child, scope_name);
                break;
            case ParseArgs:
                check_parseArgs(tmp->child, scope_name);
                break;
            
            default :
                check_program(tmp->child, scope_name);
                check_program(tmp->brother, scope_name);  
                break;      
        }
    }
}

void check_function_invocation(Structure* node ,char* scope_name){
    Scope_element* scope = get_scope(node->token->val); // Bem?
    if(scope != NULL){
        for(i=0;i<scope.number_of_params){
            //[IS THIS RIGHT?]
            basic_type tmp = check_expression(node->child,scope_name);
            if(scope->variables->type == tmp) // Parameter type esta de acordo com a expressao na invocacao
            else{
                node->error = (char*)malloc(100 * sizeof(char));
                //VER ISTO.. WTF?!
                //De acordo com exemplos é assim mas nao faz sentido?
                sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
            }  
        }
    }
    else{
        node->error = (char*)malloc(100 * sizeof(char));
        sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
    }   
    

}

void check_parseArgs(Structure* node ,char* scope_name){
    
    Table_element* id =  search_variable(node->token->val,scope_name);
    if(id != NULL){  //variable exists
        basic_type temp = check_expression(node->brother,scope_name)
        //Int  -> Atoi (args[int])
        if(id->type == integer && temp == integer); //Assign Types are correct
        //[NEEDS DOING] --> Meter boolean a true de ser usado  
        else{
            node->error = (char*)malloc(100 * sizeof(char));
            sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to types %s, %s",node->token->l,node->token->col,node->token->val,type_to_string(id->type),type_to_string(temp));
        }
    }
    else{  //variable doesn´t exist locally or globally
        node->error = (char*)malloc(100 * sizeof(char));
        sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
    }
}

void check_statement(Structure* node ,char* scope_name){

    if(strcmp(node->token->val, "If")==0 || strcmp(node->token->val, "For")==0) {
        basic_type tmp = check_expression(node->child,scope_name);
        if (tmp != boolean){
            node->error = (char*)malloc(100 * sizeof(char));
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
            if(tmp.type == scope.type);
            else{
                node->error = (char*)malloc(100 * sizeof(char));
                sprintf(node->error,"Line %d, column %d: Incompatible type %s in %s statement",node->token->l,node->token->col,type_to_string(tmp),node->token->val); 
            }
        }
    }
    else{
        check_program(node->child, scope_name);
        check_program(node->brother, scope_name);  
    }
}

void check_variable(Structure* node ,char* scope_name){

    Table_element* new = insert_variable(node->token->val, scope_name);
    
    if(new == NULL){
        node->error = (char*)malloc(100 * sizeof(char));
        sprintf(node->error,"Line %d, column %d: Symbol %s already defined",node->token->l,node->token->col,node->token->val);
    }
    return;
}

void check_assign(Structure* node, char* scope_name){

    Table_element* id =  search_variable(node->token->val,scope_name);
    if(id != NULL){  //variable exists
        basic_type tmp = check_expression(node->brother,scope_name);
        if(id->type == tmp); //Assign Types are correct
        //[NEEDS DOING] -> meter used_boolean = true;
        else{
            node->error = (char*)malloc(100 * sizeof(char));
            sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to types %s, %s",node->token->l,node->token->col,node->token->val,type_to_string(id->type),type_to_string(tmp));
        }
    }
    else{  //variable doesn´t exist locally or globally
        node->error = (char*)malloc(100 * sizeof(char));
        sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,node->token->val);
    }
    return;
}

void type_to_node(Structure* node, basic_type type){
    char* boolean_options[9] ={"Or","And","Lt","Gt","Le","Ge","Eq","Ne","Not"};
    for(i=0;i< sizeof(boolean_options)/sizeof(boolean_options[0];i++)){
        if(options[i] != NULL && strcmp(node->token->val,boolean_options[i])==0){
            node->value_type = boolean;
        }
    }

    char* boolean_options[7] ={"Add","Sub","Mul","Div","Mod","Minus","Plus"};
    for(i=0;i< sizeof(boolean_options)/sizeof(boolean_options[0];i++)){
        if(options[i] != NULL && strcmp(node->token->val,boolean_options[i])==0){
            node->value_type = type;
        }
    }
    return;
    /*switch (node->token->val){
        case "Or":
        case "And":
        case "Lt":
        case "Gt":
        case "Le":
        case "Ge":
        case "Eq":
        case "Ne":
        case "Not":
            node->value_type = boolean;
            break;
        case "Add":
        case "Sub":
        case "Mul":
        case "Div":
        case "Mod":
        case "Minus":
        case "Plus":
            node->value_type = type;
            break;
        default:
            break;
    }*/
}
void type_error(Structure* node,basic_type t1){
    
    node->error = (char*)malloc(100 * sizeof(char));
    if(strcmp(node->token->val,"Not") == 0)   sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to type %s",node->token->l,node->token->col,node->token->val,type_to_string(node->value_type));
    else    sprintf(node->error,"Line %d, column %d: Operator %s cannot be applied to types %s, %s",node->token->l,node->token->col,node->token->val,type_to_string(node->value_type),type_to_string(t1));
    return;
}

basic_type check_expression(Structure* node, char* scope_name){
    basic_type temp;
    basic_type final_type;
    if(node->child != NULL){
        temp = check_expression(node->child, scope_name);
    } 
    if(node->brother != NULL) {
        temp = check_expression(node->brother, scope_name);
    }
    if(node->child != NULL || node->brother != NULL){
        if(temp == undef) return undef;
        
        switch(node->type){
            case intlit:
                final_type = integer == temp ? integer : undef;
                if(final_type == undef) type_error(node,temp);
                break;
            case reallit:
                final_type = float32 == temp ? float32 : undef;
                if(final_type == undef) type_error(node,temp);
                break;
            case strlit:
                final_type = string == temp ? string : undef;
                if(final_type == undef) type_error(node,temp);
                break;
            case id:
                Table_element* tmp =  search_variable(node->token->val,scope_name);
                if (tmp != NULL) {  //Existe
                    final_type = tmp->type == temp ? temp : undef;  
                    if(final_type == undef) type_error(node,temp);          
                }else{
                    final_type = undef;
                    node->error = (char*)malloc(100 * sizeof(char));
                    sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,tmp);
                }
            default:
                final_type = none;
                break;
        }
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
                Table_element* tmp =  search_variable(node->token->val,scope_name);
                if (tmp != NULL) {  //Existe
                    final_type = tmp->type;            
                }else{
                    final_type = undef;
                    node->error = (char*)malloc(100 * sizeof(char));
                    sprintf(node->error,"Line %d, column %d: Cannot find symbol %s",node->token->l,node->token->col,tmp);
                }
            default:
                final_type = none;
                break;
        }
        type_to_node(node,final_type);
        return final_type;
    }
}