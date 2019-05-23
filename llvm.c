#include "structures.h"
#include "functions.h"
#include "llvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Clang 3.8.1 -> segfaults appear on llvm

int produce(Structure *node, char* scope_name,int* count_label, int* count ,int needs_return){
    //NAO ESQUECER
    //   --> produce_declarations("global");

    if(node == NULL) return needs_return;
    Structure* tmp = node;
    switch(tmp->type) {
        case Assign:
            produce_assign(tmp, scope_name,count);
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return);
            break;
        case FuncDecl:
            produce_header(tmp->child->child, tmp->child->child->token->val);
            produce_declarations(tmp->child->child->token->val);
            *count = 0;
            *count_label = 0;
            needs_return = 0;
            int n_return = produce(tmp->child, tmp->child->child->token->val, count_label, count, 0);
            if(n_return==0) printf("ret void\n");
            printf("}\n");
            produce(tmp->brother, scope_name, count_label, count, needs_return);
            break;
        case Statement:
            needs_return = produce_statement(node, scope_name, count_label, count);
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return);  
            break;
        case ParseArgs:
            produce_parse_args(node, scope_name, count);
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return);  
            break;
        default :
            needs_return = produce(tmp->child, scope_name, count_label, count, needs_return);
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return);  
            break;      
    }
    return needs_return;
}

const char* type_to_llvm(basic_type t){
    switch(t) {
        case boolean:
            return "i1";
        case float32:
            return "double";
        case string:
            return "i8*";
        default:
            return "i32";
    }
}


void produce_header(Structure* node, char* scope_name){
    if(strcmp(node->token->val,"main")==0){
        printf("define %s @main(i32, i8**) {\n",type_to_llvm(node->brother->value_type));
    }else{
        printf("define %s @%s(",type_to_llvm(node->brother->value_type),node->token->val);
        Scope_element* scope = get_scope(scope_name);
        Table_element* aux = NULL;
        if (scope != NULL){
            aux = scope->variables;
            for(int i=0; i<scope->number_of_params;i++){
                if(i == scope->number_of_params-1){
                    printf("%s) {\n",type_to_llvm(aux->type));
                }else{
                    printf("%s, ",type_to_llvm(aux->type));
                    aux = aux->next;
                }
            }
        }
        if(scope->number_of_params == 0) printf(") {\n");
    }
}

void produce_declarations(char* scope_name){
    //Percorrer tabela simbolos e dar external global ou alloca
    Special_element *special = (Special_element*) malloc(sizeof(Special_element));
    Scope_element *scope = get_scope(scope_name);
    int i = scope->number_of_params;

    char* tmp = (char*) malloc(sizeof(char)*50);
    if(scope != NULL){
        Table_element *symbol_table = scope->variables;
        Table_element *aux;
        int index=0;

        for(aux=symbol_table; aux; aux=aux->next){
            if(strcmp(scope_name, "global")==0){
                if(aux->type != function)
                    printf("@%s = global %s ",aux->name,type_to_llvm(aux->type));
                    switch(aux->type){
                        case integer:
                        printf("0\n");
                        break;
                        case boolean:
                        printf("0\n");
                        break;
                        case string:
                        printf("null\n");
                        break;
                        case float32:
                        printf("0.0\n");
                        break;
                        default:
                        break;
                    }
            }else{
                if(aux->type != function){
                    if(index<scope->number_of_params){
                        sprintf(tmp,"%d",index);
                        printf("%%%s = alloca %s\n",aux->name,type_to_llvm(aux->type));
                        printf("store %s %%%s, %s* %%%s\n",type_to_llvm(aux->type),tmp,type_to_llvm(aux->type),aux->name);
                    }else{
                        printf("%%%s = alloca %s\n",aux->name,type_to_llvm(aux->type));
                    }
                    index++;
                }
            }
        } 
    }
    return;

}


int produce_statement(Structure* node, char* scope_name, int* count_label,int* count){
    char* expr = (char*) malloc(sizeof(char)*50);

    int label_true,label_false,label_end;
    int label_condition, label_block;

    if(strcmp(node->token->val, "If")==0) {
        expr = produce_expression(node->child,scope_name,count);

        if(node->child->brother->brother->child != NULL){
            label_true = *count_label;
            label_false = *count_label = *count_label+1;
            label_end = *count_label = *count_label+1;

        printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_true,label_false);
        }else{
            label_true = *count_label;
            label_end = *count_label = *count_label+1;
        
        printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_true,label_end);
        }
        printf("label%d:\n",label_true);
        produce(node->child->brother->child,scope_name,count_label,count, 0);
        printf("br label label%d\n",label_end);
        if(node->child->brother->brother->child != NULL){
            printf("label%d:\n",label_false);
            produce(node->child->brother->brother->child,scope_name,count_label,count, 0);
            printf("br label label%d\n",label_end);
        }
        printf("label%d:\n",label_end);

    }else if(strcmp(node->token->val, "For")==0){
        if(strcmp(node->child->token->val,"Block") != 0){
            label_condition = *count_label;
            label_block = *count_label = *count_label+1; 
            label_end = *count_label = *count_label+1;
            
            printf("br label %%label%d\n",label_condition);
            printf("label%d:\n",label_condition);
            expr = produce_expression(node->child,scope_name,count);
            printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_block,label_end);
            
            printf("label%d:\n",label_block);
            produce(node->child->brother->child,scope_name,count_label,count, 0);
            printf("label%d:\n",label_condition);

        }else{
            label_block = *count_label;
            label_end = *count_label = *count_label+1;
            printf("br label %%label%d\n",label_block);
            
            printf("label%d:\n",label_block);
            produce(node->child->child,scope_name,count_label,count, 0);
            printf("br label %%label%d\n",label_block);
        }
        printf("label%d:\n",label_end);
    }else if(strcmp(node->token->val, "Return")==0){
        if(node->child == NULL){
            printf("ret void\n");
        }else{
            expr = produce_expression(node->child,scope_name,count);
            printf("ret %s %s\n",type_to_llvm(node->child->value_type),expr);
        }
        return 1;
    }else if(strcmp(node->token->val, "Print")==0){
        expr = produce_expression(node->child,scope_name,count);
        switch (node->child->value_type){
        case integer:
                printf("@.str_format = private unnamed_addr constant [4 x i8] c\"%%d\\0A\\00\"\n");
            break;
        case float32:
                printf("@.str_format = private unnamed_addr constant [4 x i8] c\".08f\\0A\\00\"\n");
            break;
        case string:
                printf("@.str_format = private unnamed_addr constant [4 x i8] c\"%%s\\0A\\00\"\n");
            break;
        case boolean:
                label_true = *count_label;
                label_false = *count_label = *count_label+1;
                label_end = *count_label = *count_label+1;
                printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_true,label_false);
                printf("label%d:\n",label_true);
                // true
                char* text_true = (char*) malloc(sizeof(char)*50);
                sprintf(text_true,".%d",*count);
                *count = *count + 1;
                printf("@.str_format = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n");
                printf("%s = getelementptr [4 x i8], [4 x i8]* @str_format, i64 0, i64 0\n", text_true);
                printf("call i32 (i8*, ...) @printf(i8* %s)\n", text_true);
                printf("br label label%d\n",label_end);
                printf("label%d:\n",label_false);
                // false
                char* text_false = (char*) malloc(sizeof(char)*50);
                sprintf(text_false,".%d",*count);
                *count = *count + 1;
                printf("@.str_format = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n");
                printf("%s = getelementptr [4 x i8], [4 x i8]* @str_format, i64 0, i64 0\n", text_false);
                printf("call i32 (i8*, ...) @printf(i8* %s)\n", text_false);
                printf("br label label%d\n",label_end);
                printf("label%d:\n",label_end);
                return 0;
            break;
        default:
                printf("@.str_format = private unnamed_addr constant [4 x i8] c\"%%s\\0A\\00\"\n");
            break;
        }
        char* str_format = (char*) malloc(sizeof(char)*50);
        sprintf(str_format,".%d",*count);
        *count = *count + 1;
        printf("%s = getelementptr [4 x i8], [4 x i8]* @str_format, i64 0, i64 0\n", str_format);
        printf("call i32 (i8*, ...) @printf(i8* %s, %s %s)\n", str_format, type_to_llvm(node->child->value_type), expr);
    }
    return 0;
}

void produce_assign(Structure* node, char* scope_name,int* count){
    char* expr = (char*) malloc(sizeof(char)*50);
    expr = produce_expression(node->child->brother,scope_name,count);
    if(node->is_global)
        printf("store %s %s, %s* @%s\n",type_to_llvm(node->value_type),expr,type_to_llvm(node->value_type),node->child->token->val);
    else
        printf("store %s %s, %s* %%%s\n",type_to_llvm(node->value_type),expr,type_to_llvm(node->value_type),node->child->token->val);
    return;
  }

char* produce_expression(Structure* node, char* scope_name, int* count){
    char* expr1 = (char*) malloc(sizeof(char)*30);
    char* expr2 = (char*) malloc(sizeof(char)*30);

    char* tmp = (char*) malloc(sizeof(char)*30);

    char* str = (char*) malloc(sizeof(char)*100);

    if(node->type == Block){
        return "";
    }else if(node->type == Call){
    
        
       if(node->child->brother == NULL) sprintf(str+strlen(str),")\n");
        for(Structure* ptr = node->child->brother;ptr != NULL;ptr = ptr->brother){
            char* expr = produce_expression(ptr, scope_name, count);
            if(ptr->brother == NULL){
                sprintf(str+strlen(str),"%s %s)\n",type_to_llvm(ptr->value_type),expr);
            }else{
                sprintf(str+strlen(str),"%s %s, ",type_to_llvm(ptr->value_type),expr);
            }
        }   

        sprintf(tmp,"%%.%d",*count);
        printf("%s = call %s @%s(%s",tmp,type_to_llvm(node->value_type),node->child->token->val,str);
        

        *count = *count + 1;
        return tmp;
    }
    
    if(node->child != NULL){
        expr1 = produce_expression(node->child, scope_name,count);
    } 
    if(node->child != NULL && node->child->brother != NULL) { 
        expr2 = produce_expression(node->child->brother, scope_name,count);
    }

    sprintf(tmp,"%%.%d",*count); //var name is %Count

    if(node->type == Expression){
        printf("%s = ",tmp);
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
            if(node->child->value_type == float32){
                printf("fcmp slt double %s %s\n", expr1, expr2);
            }else{
                printf("icmp slt %s %s %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }
            
        }else if(strcmp(node->token->val, "Gt")==0){
             if(node->child->value_type == float32){
                printf("fcmp sgt double %s %s\n", expr1, expr2);
            }else{
                printf("icmp sgt %s %s %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Le")==0){
             if(node->child->value_type == float32){
                printf("fcmp sle double %s %s\n", expr1, expr2);
            }else{
                printf("icmp sle %s %s %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Ge")==0){
             if(node->child->value_type == float32){
                printf("fcmp sge double %s %s\n", expr1, expr2);
            }else{
                printf("icmp sge %s %s %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Eq")==0){
             if(node->child->value_type == float32){
                printf("fcmp eq double %s %s\n", expr1, expr2);
            }else{
                printf("icmp eq %s %s %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Ne")==0){
             if(node->child->value_type == float32){
                printf("fcmp ne double %s %s\n", expr1, expr2);
            }else{
                printf("icmp ne %s %s %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }
        }
         
        else if(strcmp(node->token->val, "And")==0){
            printf("and i1 %s, %s\n",expr1,expr2);
            
        }else if(strcmp(node->token->val, "Or")==0){
            printf("or i1 %s, %s\n",expr1,expr2);
            
        }else if(strcmp(node->token->val, "Not")==0){
            printf("xor i1 %s, true\n",expr1);
        }

        *count = *count+1;
        return tmp;
    }else if(node->type == id){
        if(node->is_global){
            printf("%s = load %s, %s* @%s\n",tmp,type_to_llvm(node->value_type),type_to_llvm(node->value_type),node->token->val);
        }else{
            printf("%s = load %s, %s* %%%s\n",tmp,type_to_llvm(node->value_type),type_to_llvm(node->value_type),node->token->val);
        }
        *count = *count+1;
        return tmp;  
    }else{
        //casos primitivos, retorna valor || ex : add var , '2' <--
        if(node->value_type == float32){
            if(node->token->val[0] == '.'){
                char *float_str = (char *)malloc(sizeof(char) * 100);
                sprintf(float_str, "0%s", node->token->val);
                return float_str;
            }
            int str_size = strlen(node->token->val); 
            char *second_string = (char *)malloc(sizeof(char) * 100);
            int j = 0;
            for(int i=0; i<str_size; i++){
                if((node->token->val[i] == 'e' || node->token->val[i] == 'E')){
                    second_string[j] = '.';
                    second_string[j+1] = '0';
                    second_string[j+2] = 'e';
                    j += 2;
                }else{
                    second_string[j] = node->token->val[i];
                }
                j += 1;
            }
            second_string[j+1] = '\0';
            return second_string;
        }
        return node->token->val;
    }
}

void produce_parse_args(Structure *node, char* scope_name, int* count){
    
}


