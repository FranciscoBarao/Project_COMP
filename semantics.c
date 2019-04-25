#include "structures.h"
#include "semantics.h"
#include "symbol_table.h"
#include <stdio.h>



void check_program(Structure *node, char* scope_name ){
    Structure* tmp = node;
    
    if(tmp != NULL){
        switch(tmp->type) {
            case VarDecl:
                check_variable(tmp->child, scope_name); // If not  found -> create
                break;
            case Assign:
                check_assign(node, tmp->child, scope_name); 
                break;
            case FunctionInvoc:
                check_parameters(node, tmp->token->val, scope_name);
                break;
            case FuncHeader:
                check_program(node->child, tmp->token->val);
                check_program(node->brother, tmp->token->val);
                break;
            default :
                check_program(node->child, scope_name);
                check_program(node->brother, scope_name);  
                break;      
        }
    }
}


void check_variable(Structure* node ,char* scope_name){
    int error_count=0;

    Table_element* new = insert_variable(node->token->val, scope_name);
    
    if(new == NULL){
        printf("Line  %d , column %d : Symbol %s already defined!\n", node->token->val);
    }
    return;
}

void check_assign(Structure* node, char* scope_name){

    Table_element* id =  search_variable(node->child->token->val,scope_name);
    if(id != NULL){  //variable exists
       if(check_expression(node->brother, id)) /*Meter boolean a true de ser usado*/ ; //Assign Types are correct
    }
    else{  //variable doesn´t exist locally or globally
        printf("Line  %d , column %d: Cannot find symbol %s\n", node->child->token->val);
    }
    return;
}

void check_expression(Structure* node, Table_element* id_brother, char* scope_name){
    
    if(node->child   != NULL)   check_expression(node->child, id_brother, scope_name);
    if(node->brother != NULL)   check_expression(node->brother, id_brother, scope_name);


    if(node->token->type == id){
        Table_element* tmp =  search_variable(node->token->val,scope_name);
        if (tmp != NULL) {  //Existe

            if(tmp.type == id_brother.type){ //Tipo igual ao lado esquerdo
                
            }
            else printf("Line  %d , column %d: Incompatible type %s in %s statement\n", temp->type);
            
        }else{

        }
    }

    else{
        
    }
 return;
}