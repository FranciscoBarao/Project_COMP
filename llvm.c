#include "structures.h"
#include "functions.h"
#include "semantics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* ifcond = 0;

int* count = 0;

void check_program(Structure *node, char* scope_name ){ //First run of tree
    if(node == NULL) return;
    Structure* tmp = node;


    switch(tmp->type) {
        case Assign:
            produce_assign(tmp, scope_name);
            check_program(tmp->brother, scope_name);
            break;
        case VarDecl:
            check_program(tmp->brother, scope_name);  
            break;
        case FuncHeader:
            //define i32 main() {
            produce_header(tmp->child,scope_name);
            //body of function
            check_program(tmp->child, tmp->child->token->val);
            // }
            printf("\n}");
            //continues going through tree
            check_program(tmp->brother, tmp->child->token->val);
            break;
        case ParamDecl:
            check_program(tmp->brother, scope_name);
            break;
        default :
            check_program(tmp->child, scope_name);
            check_program(tmp->brother, scope_name);  
            break;      
    }
}

const char* type_to_llvm(Structure* node ){
    basic_type t = node->value_type;
    switch(t) {
        case integer:
            return "i32";
        case boolean:
            return "i1";
        case float32:
            return "double";
        default:
            return "";
    }
}


void produce_header(Structure* node, char* scope_name){

    printf("define %s @%s() {",type_to_llvm(node->brother),node->token->val);
}

void produce_statement(Structure* node, char* scope_name){
    if(strcmp(node->token->val, "If")==0 ) {
        //produce_expression(v1)
        //produce_expression(v2)
        //Tem de retornar o contador de temporarios para fazer Tcount para imprimir
        
        //%name = icmp eq i32 %v1, %v2
        //i32? why mesmo?
        printf("%ifcond%d = icmp i32 %s %s %s",ifcond,node->child->token->val);
        //integer of amount of ifs..? not sure this is the way..
        
        //br i1 %name, label %name2, label %name3

        printf("br i1 %ifcond%d, label %%s, label %%s",ifcond);

        ifcond++; 
    }
}
void produce_assign(Structure* node, char* scope_name){
    char* expr ;
    printf("%%s = add %s 0, %s", node->child->token->val, node->value_type,expr);

}


char* produce_expression(Structure* node, char* scope_name){
    
    char* expr1 = (char*) malloc(sizeof(char)*50);
    char* expr2 = (char*) malloc(sizeof(char)*50);
    //Temporary value holder
    char* tmp = (char*) malloc(sizeof(char)*50);

    if(node->type == Block){
        return none;
    }else if(node->type == Call){
        //check_function_invocation(node, scope_name);
        //return node->value_type;
    }else if(node->type == id){
        //Return name of variable..
        return node->token->val;  
    }

    if(node->child != NULL){
        expr1 = produce_expression(node->child, scope_name);
    } 
    if(node->brother != NULL) { 
        expr2 = produce_expression(node->brother, scope_name);
    }

    sprintf(tmp,"%d",count); //var name is %Count
    if(strcmp(node->token->val, "Add")==0){ 
        printf("%%s = add %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
        //Count into string? return temporal variable? 
        return count->toString() ?
    }else if(strcmp(node->token->val, "Sub")==0){
        printf("%%s = sub %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Mul")==0){
        printf("%%s = mul %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Div")==0){
        //udiv ou sdiv?
        printf("%%s = udiv %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Mod")==0){
        //urem ou srem?
        printf("%%s = urem %s %s, %s\n",tmp,node->value_type,expr1,expr2);   
        count++;     
    }else if(strcmp(node->token->val, "Minus")==0){
        printf("%%s = sub %s 0, %s\n",tmp,node->value_type,expr1);   
        count++;
    }
    
    //Check bellow
    else if(strcmp(node->token->val, "Lt")==0){
        //udiv ou sdiv?
        printf("icmp slt %s %s %s\n",type_to_llvm(node->value_type),expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Gt")==0){
        //udiv ou sdiv?
        printf("%%s = udiv %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Le")==0){
        //udiv ou sdiv?
        printf("%%s = udiv %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Ge")==0){
        //udiv ou sdiv?
        printf("%%s = udiv %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Eq")==0){
        //udiv ou sdiv?
        printf("%%s = udiv %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }else if(strcmp(node->token->val, "Ne")==0){
        //udiv ou sdiv?
        printf("%%s = udiv %s %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;
    }


    //Check below
    else if(strcmp(node->token->val, "And")==0){
        printf("%%s = and i1 %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;   
        
    }else if(strcmp(node->token->val, "Or")==0){
        printf("%%s = or i1 %s, %s\n",tmp,node->value_type,expr1,expr2);
        count++;   
        
    }else if(strcmp(node->token->val, "Not")==0){
        
    }else{
        //casos primitivos, retorna valor ..
        //ex : add var , 2 <--
        return node->token->val;
    
    }



}