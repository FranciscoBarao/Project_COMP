#include "structures.h"
#include "functions.h"
#include "llvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int produce(Structure *node, char* scope_name,int* count_label, int* count, int needs_return){

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
            int n_return = produce(tmp->child, tmp->child->child->token->val, count_label, count, 0 );
            if(n_return==0){
                switch (val_to_basic(node->child->child->brother->token->val)){
                case none:
                    printf("ret i32 0\n");
                    break;
                case integer:
                    printf("ret i32 0\n");
                    break;
                case float32:
                    printf("ret double 0.0\n");
                    break;
                default:
                    printf("ret i32 0\n");
                    break;
                }
            }
            printf("}\n");
            produce(tmp->brother, scope_name, count_label, count, needs_return );
            break;
        case Statement:
            needs_return = produce_statement(node, scope_name, count_label, count );
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return );  
            break;
        case ParseArgs:
            produce_parse_args(node, scope_name, count);
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return );  
            break;
        case Call:
            produce_call(node,scope_name,count);
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return );
            break;
        default :
            needs_return = produce(tmp->child, scope_name, count_label, count, needs_return );
            needs_return = produce(tmp->brother, scope_name, count_label, count, needs_return );  
            break;      
    }
    return needs_return;
}

const char* type_to_llvm(basic_type t){
    switch(t) {
        case function:
            return "FUNCTION";
        case undef:
            return "NONE";
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
        
        printf("define %s @main(i32, i8**) {\n",type_to_llvm(val_to_basic(node->brother->token->val)));
    }else{

        printf("define %s @%s(",type_to_llvm(val_to_basic(node->brother->token->val)),node->token->val);
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
    Scope_element *scope = get_scope(scope_name);

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
                    if(index < scope->number_of_params){
                        sprintf(tmp,"%d",index);
                        printf("%%%s = alloca %s\n",aux->name,type_to_llvm(aux->type));
                        printf("store %s %%%s, %s* %%%s\n",type_to_llvm(aux->type),tmp,type_to_llvm(aux->type),aux->name);
                    }else{
                        printf("%%%s = alloca %s\n",aux->name,type_to_llvm(aux->type));
                        switch(aux->type){
                            case integer:
                                printf("store %s 0, %s* %%%s\n",type_to_llvm(aux->type),type_to_llvm(aux->type),aux->name);

                            break;
                            case float32:
                                printf("store %s 0.0, %s* %%%s\n",type_to_llvm(aux->type),type_to_llvm(aux->type),aux->name);
                            break;
                        }

                        
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
            label_false = *count_label + 1;
            label_end = *count_label + 2;
            *count_label = *count_label+3;

        printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_true,label_false);
        }else{
            label_true = *count_label;
            label_end = *count_label + 1;
            *count_label = *count_label+2;

        
        printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_true,label_end);
        }
        printf("label%d:\n",label_true);
        produce(node->child->brother->child,scope_name,count_label,count, 0 );
        printf("br label %%label%d\n",label_end);
        if(node->child->brother->brother->child != NULL){
            printf("label%d:\n",label_false);
            produce(node->child->brother->brother->child,scope_name,count_label,count, 0 );
            printf("br label %%label%d\n",label_end);
        }
        printf("label%d:\n",label_end);


    }else if(strcmp(node->token->val, "For")==0){
        if(strcmp(node->child->token->val,"Block") != 0){
            label_condition = *count_label;
            label_block = *count_label + 1;
            label_end = *count_label + 2;
            *count_label = *count_label+3;
            
            printf("br label %%label%d\n",label_condition);
            printf("label%d:\n",label_condition);
            expr = produce_expression(node->child,scope_name,count);
            printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_block,label_end);
            
            printf("label%d:\n",label_block);

            produce(node->child->brother->child,scope_name,count_label, count, 0 );
            printf("br label %%label%d\n",label_condition);

        }else{
            label_block = *count_label;
            label_end = *count_label + 1;
            *count_label = *count_label+2;
            printf("br label %%label%d\n",label_block);
            
            printf("label%d:\n",label_block);
            produce(node->child->child,scope_name,count_label,count, 0 );
            printf("br label %%label%d\n",label_block);
        }
        printf("label%d:\n",label_end);
    }else if(strcmp(node->token->val, "Return")==0){
        if(node->child == NULL){
            printf("ret i32 0\n");
        }else{
            expr = produce_expression(node->child,scope_name,count);
            printf("ret %s %s\n",type_to_llvm(node->child->value_type),expr);
        }
        return 1;


    }else if(strcmp(node->token->val, "Print")==0){
        expr = produce_expression(node->child,scope_name,count);
        if(node->child->type == strlit || node->child->type == intlit){
            int size = node->child->is_global;
            char* str = (char*) malloc(sizeof(char)*50);
            sprintf(str,"%%.%d",*count);
            *count = *count + 1;
            printf("%s = getelementptr [%d x i8], [%d x i8]* %s,  i32 0,  i32 0\n", str, size, size, node->child->token->val);
            printf("call i32 (i8*, ...) @printf(i8* %s)\n", str);
            return 0;
        }
        if(node->child->value_type == boolean){
            label_true = *count_label;
            label_false = *count_label + 1;
            label_end = *count_label +2;
            *count_label = *count_label+3;
            
            printf("br i1 %s, label %%label%d, label %%label%d\n",expr,label_true,label_false);
            printf("label%d:\n",label_true);
            // true
            char* text_true = (char*) malloc(sizeof(char)*50);
            sprintf(text_true,"%%.%d",*count);
            *count = *count + 1;
            printf("%s = getelementptr [6 x i8], [6 x i8]* @.true,  i32 0,  i32 0\n", text_true);
            printf("call i32 (i8*, ...) @printf(i8* %s)\n", text_true);
            printf("br label %%label%d\n",label_end);
            printf("label%d:\n",label_false);
            // false
            char* text_false = (char*) malloc(sizeof(char)*50);
            sprintf(text_false,"%%.%d",*count);
            *count = *count + 1;
            printf("%s = getelementptr [7 x i8], [7 x i8]* @.false,  i32 0,  i32 0\n", text_false);
            printf("call i32 (i8*, ...) @printf(i8* %s)\n", text_false);
            printf("br label %%label%d\n",label_end);
            printf("label%d:\n",label_end);
            return 0;
        }
        char* str = (char*) malloc(sizeof(char)*50);
        sprintf(str,"%%.%d",*count);
        *count = *count + 1;
        switch (node->child->value_type){
            case integer:
                    printf("%s = getelementptr [4 x i8], [4 x i8]* @.integer,  i32 0,  i32 0\n", str);
                break;
            case float32:
                    printf("%s = getelementptr [7 x i8], [7 x i8]* @.float,  i32 0,  i32 0\n", str);
                break;
            case string:
                    printf("%s = getelementptr [4 x i8], [4 x i8]* @.string,  i32 0,  i32 0\n", str);
                break;
            default:
                printf("%s = getelementptr [4 x i8], [4 x i8]* @.string,  i32 0,  i32 0\n", str);
                break;

        }
        printf("call i32 (i8*, ...) @printf(i8* %s, %s %s)\n", str, type_to_llvm(node->child->value_type), expr);
    }
    return 0;
}

void produce_assign(Structure* node, char* scope_name,int* count){
    char* expr = (char*) malloc(sizeof(char)*50);
    expr = produce_expression(node->child->brother,scope_name,count);
    if(node->child->is_global == 1)
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
        
       if(node->child->brother == NULL) {
           sprintf(str+strlen(str),")\n");
           }
        for(Structure* ptr = node->child->brother;ptr != NULL;ptr = ptr->brother){
            char* expr = produce_expression(ptr, scope_name, count);
            if(ptr->brother == NULL){
                sprintf(str+strlen(str),"%s %s)\n",type_to_llvm(ptr->value_type),expr);
            }else{
                sprintf(str+strlen(str),"%s %s, ",type_to_llvm(ptr->value_type),expr);
            }
        }   

        sprintf(tmp,"%%.%d",*count);
        if(node->value_type == none){
            printf("call %s @%s(%s",type_to_llvm(node->value_type),node->child->token->val,str);
        }else{
            printf("%s = call %s @%s(%s",tmp,type_to_llvm(node->value_type),node->child->token->val,str);
            *count = *count + 1;
        }
        
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
        if(strcmp(node->token->val, "Add")==0){ 
            if(node->child->value_type == float32){
                printf("fadd %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }else{
                printf("add %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }

        }else if(strcmp(node->token->val, "Sub")==0){
            if(node->child->value_type == float32){
                printf("fsub %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }else{
                printf("sub %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }

        }else if(strcmp(node->token->val, "Mul")==0){
            if(node->child->value_type == float32){
               printf("fmul %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }else{
                printf("mul %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }

        }else if(strcmp(node->token->val, "Div")==0){
            if(node->child->value_type == float32){
                printf("fdiv %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }else{
                printf("sdiv %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);
            }

        }else if(strcmp(node->token->val, "Mod")==0){
            if(node->child->value_type == float32){
                printf("frem %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);   
            }else{
                printf("srem %s %s, %s\n",type_to_llvm(node->value_type),expr1,expr2);   
            }

        }else if(strcmp(node->token->val, "Minus")==0){
            if(node->child->value_type == float32){
               printf("fsub %s 0.0, %s\n",type_to_llvm(node->value_type),expr1);   
            }else{
                printf("sub %s 0, %s\n",type_to_llvm(node->value_type),expr1);   
            }

        }else if(strcmp(node->token->val, "Plus")==0){
            if(node->child->value_type == float32){
                printf("fadd %s 0.0, %s\n",type_to_llvm(node->value_type),expr1);   
            }else{
                printf("add %s 0, %s\n",type_to_llvm(node->value_type),expr1);   
            }
        }
        

        else if(strcmp(node->token->val, "Lt")==0){
            if(node->child->value_type == float32){
                printf("fcmp olt double %s, %s\n", expr1, expr2);
            }else{
                printf("icmp slt %s %s, %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }
            
        }else if(strcmp(node->token->val, "Gt")==0){
             if(node->child->value_type == float32){
                printf("fcmp ogt double %s, %s\n", expr1, expr2);
            }else{
                printf("icmp sgt %s %s, %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Le")==0){
             if(node->child->value_type == float32){
                printf("fcmp ole double %s, %s\n", expr1, expr2);
            }else{
                printf("icmp sle %s %s, %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Ge")==0){
             if(node->child->value_type == float32){
                printf("fcmp oge double %s, %s\n", expr1, expr2);
            }else{
                printf("icmp sge %s %s, %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Eq")==0){
             if(node->child->value_type == float32){
                printf("fcmp oeq double %s, %s\n", expr1, expr2);
            }else{
                printf("icmp eq %s %s, %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
            }

        }else if(strcmp(node->token->val, "Ne")==0){
             if(node->child->value_type == float32){
                printf("fcmp one double %s, %s\n", expr1, expr2);
            }else{
                printf("icmp ne %s %s, %s\n", type_to_llvm(node->child->value_type), expr1, expr2);
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
        if(node->is_global==1){
            printf("%s = load %s, %s* @%s\n",tmp,type_to_llvm(node->value_type),type_to_llvm(node->value_type),node->token->val);
        }else{
            printf("%s = load %s, %s* %%%s\n",tmp,type_to_llvm(node->value_type),type_to_llvm(node->value_type),node->token->val);
        }
        *count = *count+1;
        return tmp;  
    }else{
        //casos primitivos, retorna valor || ex : add var , '2' <--
        if(node->value_type == float32){
            int needs_dot = 1;
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
                    needs_dot = 0;
                    second_string[j] = '.';
                    second_string[j+1] = '0';
                    second_string[j+2] = 'e';
                    j += 2;
                }else{
                    second_string[j] = node->token->val[i];
                }
                j += 1;
            }
            if(needs_dot == 1){
                second_string[++j] = '.';
                second_string[++j] = '0';
            }
            second_string[j+1] = '\0';
            return second_string;

        }

        if(node->value_type == integer){
            char *t = (char *)malloc(sizeof(char) * 50);
            if(node->token->val[0] == '0'){
                if((node->token->val[1] == 'x' || node->token->val[1] == 'X')){
                    sprintf(t, "%ld",strtol(node->token->val, NULL, 0));
                }else{
                    sprintf(t, "%ld",strtol(node->token->val,NULL,8));
                }
                return t;
            }
        }
        return node->token->val;
    }
}

void produce_parse_args(Structure *node, char* scope_name, int* count){

    char* expr_brother = (char*) malloc(sizeof(char)*150);
    expr_brother = produce_expression(node->child->brother, scope_name, count);
    char* tmp = (char*) malloc(sizeof(char)*30);
    sprintf(tmp,"%%.%d",*count);
    *count = *count + 1;
    printf("%s = alloca i8**\n", tmp);
    printf("store i8** %%1, i8*** %s\n", tmp);
    char* tmp2 = (char*) malloc(sizeof(char)*30);
    sprintf(tmp2,"%%.%d",*count);
    *count = *count + 1;
    printf("%s = load i8**, i8*** %s\n", tmp2, tmp);
    char* tmp3 = (char*) malloc(sizeof(char)*30);
    sprintf(tmp3,"%%.%d",*count);
    *count = *count + 1;
    printf("%s = getelementptr inbounds i8*, i8** %s,  i32 %s\n", tmp3, tmp2, expr_brother);
    char* tmp4 = (char*) malloc(sizeof(char)*30);
    sprintf(tmp4,"%%.%d",*count);
    *count = *count + 1;
    printf("%s = load i8*, i8** %s\n", tmp4, tmp3);
    char* tmp5 = (char*) malloc(sizeof(char)*30);
    sprintf(tmp5,"%%.%d",*count);
    *count = *count + 1;
    printf("%s = call i32 @atoi(i8* %s)\n", tmp5, tmp4);
    printf("store i32 %s, i32* %%%s\n", tmp5, node->child->token->val);
}

void init_produce(Structure *node, int* count_str){
    printf("declare i32 @printf(i8*, ...)\n");
    printf("declare i32 @atoi(i8*)\n");
    printf("@.true = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n");
    printf("@.false = private unnamed_addr constant [7 x i8] c\"false\\0A\\00\"\n");
    printf("@.integer = private unnamed_addr constant [4 x i8] c\"%%d\\0A\\00\"\n");
    printf("@.string = private unnamed_addr constant [4 x i8] c\"%%s\\0A\\00\"\n");
    printf("@.float = private unnamed_addr constant [7 x i8] c\"%%.08f\\0A\\00\"\n");
    printf("@.empty = private unnamed_addr constant [2 x i8] c\"\\0A\\00\"\n");
    Str_meta4 *pointer = (Str_meta4*)malloc(sizeof(Str_meta4));
    change_str(node, pointer, count_str);
    Str_meta4 *aux = pointer->next;
    while(aux != NULL){
        printf("%s", aux->text);
        aux = aux->next;
    }
}

void change_str(Structure* node, Str_meta4 *pointer, int* count_str){
    if(node == NULL) return;
    if(node->type == Statement){
        if(strcmp(node->token->val, "Print")==0){
            if(node->child->type == strlit || node->child->type == intlit){
                int random_stuff = 0;
                int str_size = strlen(node->child->token->val);
                char* str_final = (char*) malloc(sizeof(char)*200);
                int j=0;
                for(int i=0; i<str_size-1; i++){
                    if(node->child->token->val[i] == '\\' ){
                        str_final[j] = node->child->token->val[i];
                        switch (node->child->token->val[i+1]){
                        case 'f':
                            str_final[++j] = '0';
                            str_final[++j] = 'C';
                            break;
                        case 'n':
                            str_final[++j] = '0';
                            str_final[++j] = 'A';
                            break;
                        case 'r':
                            str_final[++j] = '0';
                            str_final[++j] = 'D';
                            break;
                        case 't':
                            str_final[++j] = '0';
                            str_final[++j] = '9';
                            break;
                        case '\\':
                            str_final[++j] = '5';
                            str_final[++j] = 'C';
                            break;
                        case '\"':
                            str_final[++j] = '2';
                            str_final[++j] = '2';
                            break;
                        default:
                            break;
                        }
                        random_stuff += 1;
                        i = i + 1;
                    }else if(node->child->token->val[i] == '%'){
                        str_final[j] = node->child->token->val[i];
                        str_final[++j] = '%';
                        random_stuff -= 1;
                    }else
                    {
                        str_final[j] = node->child->token->val[i];
                    }
                    j += 1;   
                }
                str_final[j] = '\0';
                Str_meta4* new = (Str_meta4*) malloc(sizeof(Str_meta4));
                char* aux = (char*) malloc(sizeof(char)*50);
                sprintf(aux,"@.str%d",*count_str);
                *count_str = *count_str + 1;
                sprintf(new->text, "%s = private unnamed_addr constant [%d x i8] c%s\\0A\\00\"\n",aux,str_size-random_stuff,str_final);
                new->next = NULL;
                Str_meta4* new2 = pointer;
                while(new2->next != NULL){
                    new2 = new2->next;
                }
                new2->next = new;
                strcpy(node->child->token->val, aux);
                node->child->is_global = str_size-random_stuff;
            }
        }else{
            change_str(node->child, pointer, count_str);
        }
    }else{
        change_str(node->child, pointer, count_str);
    }
    change_str(node->brother, pointer, count_str);
}

void produce_call(Structure* node,char* scope_name, int* count){
    char* str = (char*) malloc(sizeof(char)*50);
    char* tmp = (char*) malloc(sizeof(char)*50);

    if(node->child->brother == NULL) {
           sprintf(str+strlen(str),")\n");
           }
        for(Structure* ptr = node->child->brother;ptr != NULL;ptr = ptr->brother){
            char* expr = produce_expression(ptr, scope_name, count);
            if(ptr->brother == NULL){
                sprintf(str+strlen(str),"%s %s)\n",type_to_llvm(ptr->value_type),expr);
            }else{
                sprintf(str+strlen(str),"%s %s, ",type_to_llvm(ptr->value_type),expr);
            }
        }   

         sprintf(tmp,"%%.%d",*count);
        if(node->value_type == none){
            printf("call %s @%s(%s",type_to_llvm(node->value_type),node->child->token->val,str);
        }else{
           
            printf("%s = call %s @%s(%s",tmp,type_to_llvm(node->value_type),node->child->token->val,str);
            *count = *count + 1;
        }
}
