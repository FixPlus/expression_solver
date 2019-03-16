#include "problem_st2.h"

// Grammar:
// logic ::= comp {&&, ||} logic | comp
// comp ::= expr {<,>,==, != etc.} expr | expr;
// expr ::= mult {+, -} expr | mult
// mult ::= term {*, /} mult | term
// term ::= ( logic ) | number



struct node_t *
build_syntax_tree(struct lexem_t** lex, struct var_table_t table) {	
	//printf("\nBuilding syntax tree...\nTotal lexems: %d\n",lexarr.capacity - 1);
	struct lexem_t* enter_point = *lex;
	struct node_t* ret = parse_logic(lex, table);
	*lex = enter_point;
	
	*lex = *lex + 1;
	
	if(!ret)
		printf("Parse failed: there are some syntax mistakes!\n");
	
	return ret;
}

struct node_t* parse_logic(struct lexem_t** lex, struct var_table_t table){
	struct node_t* left = parse_comparation(lex, table);
	if(!left)
		return NULL;
	
	if(is_logic(**lex)){
		enum operation_t opcode = (**lex).lex.op; 

		*lex = *lex - 1;
		
		struct node_t* right = parse_logic(lex, table);
		
		if(!right){
			free_syntax_tree(left);
			return NULL;
		}
		
		struct node_t* expr = create_operation_expression(opcode);
		
		expr->left = right;
		expr->right = left;
		return expr;
		
	}
	else
		return left;
}

struct node_t* parse_comparation(struct lexem_t** lex, struct var_table_t table){
	struct node_t* left = parse_expr(lex, table);
	if(!left)
		return NULL;
	
	if(is_compare(**lex)){
		enum operation_t opcode = (**lex).lex.op; 

		*lex = *lex - 1;
		
		struct node_t* right = parse_expr(lex, table);
		
		if(!right){
			free_syntax_tree(left);
			return NULL;
		}
		
		struct node_t* expr = create_operation_expression(opcode);
		
		expr->left = right;
		expr->right = left;
		return expr;
		
	}
	else
		return left;
}
struct node_t* parse_expr(struct lexem_t** lex, struct var_table_t table){
	struct node_t* left= parse_mult(lex, table);
	if(!left)
		return NULL;
	
	if(is_plusminus(**lex)){
		
		enum operation_t opcode = (**lex).lex.op; 
		
		*lex = *lex - 1;
		
		struct node_t* right = parse_expr(lex, table);
		
		if(!right){
			free_syntax_tree(left);
			return NULL;
		}
		
		struct node_t* expr = create_operation_expression(opcode);
		
		expr->left = right;
		expr->right = left;
		return expr;
	}
	else
			return left;
}

int is_logic(struct lexem_t lex){
	return (lex.kind == OP && (lex.lex.op == LG_AND || lex.lex.op == LG_OR)) ? 1:0;
}

int is_compare(struct lexem_t lex){
	return (lex.kind == OP && (lex.lex.op == EQUAL || lex.lex.op == NOT_EQUAL
							|| lex.lex.op == MORE || lex.lex.op == LESS
							|| lex.lex.op == EQ_OR_MORE || lex.lex.op == EQ_OR_LESS)) ? 1:0;
}

int is_plusminus(struct lexem_t lex){
	return (lex.kind == OP && (lex.lex.op == ADD || lex.lex.op == SUB)) ? 1:0;
}

int is_muldiv(struct lexem_t lex){
	return (lex.kind == OP && (lex.lex.op == MUL || lex.lex.op == DIV)) ? 1:0;
}

int is_brace(struct lexem_t lex){
	return lex.kind == BRACE ? 1:0;
}

int is_num(struct lexem_t lex){
	return lex.kind == NUM ? 1:0;
}

int is_assign(struct lexem_t lex){
	return lex.kind == ASSIGN ? 1:0;
}

int is_var(struct lexem_t lex){
	return lex.kind == VAR ? 1:0;
}
int is_end(struct lexem_t lex){
	return lex.kind == END ? 1:0;
}
int is_semicolon(struct lexem_t lex){
	return lex.kind == SEMICOLON ? 1:0;
}
struct node_t* create_operation_expression(enum operation_t opcode){
	struct node_t* expr = (struct node_t*)calloc(1,sizeof(struct node_t));
	expr->data.k = NODE_OP;
	expr->data.u.op = opcode;
	return expr;
}

struct node_t* parse_mult(struct lexem_t** lex, struct var_table_t table){
	struct node_t* left= parse_term(lex, table);
	if(!left)
		return NULL;
	
	
	if(is_muldiv(**lex)){
		
		enum operation_t opcode = (**lex).lex.op;
		*lex = *lex - 1;
		
		struct node_t* right = parse_mult(lex, table);
		
		if(!right){
			free_syntax_tree(left);
			return NULL;
		}
		
		struct node_t* expr = create_operation_expression(opcode);

		expr->left = right;
		expr->right = left;
		return expr;
	}
	else
		return left;	
}

struct node_t* parse_term(struct lexem_t** lex, struct var_table_t table){
	
	if(check_end(*lex)){     //checking if we unexpectly reach the end of expression (e.g. mistakes in lexems). In case when something goes wrong we safely stop building
		printf("Unexpected reach of the end of expression!\n");
		return NULL;
	}
	
	
	struct node_t* ret = (struct node_t*)calloc(1,sizeof(struct node_t));
	
	
	switch((**lex).kind){
		case BRACE:{
			*lex = *lex - 1;
			free(ret);
			ret = parse_logic(lex, table);
			if(!is_brace(**lex)){   //if expression didn't end with brace, stopping the tree building
				free_syntax_tree(ret);
				printf("Braces are not correct!\n");
				return NULL;
			}
			*lex = *lex - 1;
			return ret;
		}
	
		case NUM: {
			ret->data.k = NODE_VAL;
			ret->data.u.d = (**lex).lex.num;
			*lex = *lex - 1;
			return ret;
		}
		case VAR: {
			struct var_t var = get_var(table, (**lex).lex.name);
			if(is_undef(var)){
				free_syntax_tree(ret);
				throw_the_undef_var_exception((**lex).lex.name);
				return NULL;				
			}
			ret->data.k = NODE_VAL;
			ret->data.u.d = var.value;
			*lex = *lex - 1;
			return ret;
		
		}
		default:{
			free(ret);
			printf("Unnamed error!\n");
			return NULL;
		}
	}
}

int check_end(struct lexem_t* lex){
	return (lex->kind == OP || lex->kind == NUM || lex->kind == VAR || lex->kind == BRACE) ? 0 : 1;
}

int calc_result(struct node_t *top) {
	printf("Calculating...\n");
	
	struct calc_data_t result = calculate(top);
	if(result.stat == CALC_FAILED){
		printf("Calculation failed!\n");
		return 0;
	}
	else
		return result.result;
	
}

struct calc_data_t calculate(struct node_t *top){
	struct calc_data_t ret = {CALC_SUCCES, 0};
	
	if(top->data.k == NODE_VAL){
		ret.result = top->data.u.d; 
		return ret;
	}
	
	struct calc_data_t res1 = calculate(top->left);
	struct calc_data_t res2 = calculate(top->right);
	
	if(res1.stat == CALC_FAILED || res2.stat == CALC_FAILED){
		ret.stat = CALC_FAILED;
		return ret;
	}
	
	switch(top->data.u.op){
		case ADD:{
			ret.result = res1.result + res2.result;
			return  ret;
		}
		case SUB:{
			ret.result = res1.result - res2.result;
			return  ret;
		}
		case MUL:{
			ret.result = res1.result * res2.result;
			return  ret;
		}
		case DIV:{
			if(res2.result == 0){
				ret.stat = CALC_FAILED;
				return ret;
			}
			else{
				ret.result = res1.result / res2.result;
				return ret;
			}
		}
		case EQUAL:{
			ret.result = (res1.result == res2.result) ? 1:0;
			return ret;
		}
		case NOT_EQUAL:{
			ret.result = (res1.result != res2.result) ? 1:0;
			return ret;
		}
		case MORE:{
			ret.result = (res1.result > res2.result) ? 1:0;
			return ret;
		}
		case LESS:{
			ret.result = (res1.result < res2.result) ? 1:0;
			return ret;
		}
		case EQ_OR_MORE:{
			ret.result = (res1.result >= res2.result) ? 1:0;
			return ret;
		}
		case EQ_OR_LESS:{
			ret.result = (res1.result <= res2.result) ? 1:0;
			return ret;
		}
		case LG_AND:{
			ret.result = (res1.result && res2.result) ? 1:0;
			return ret;
		}
		case LG_OR:{
			ret.result = (res1.result || res2.result) ? 1:0;
			return ret;
		}

		default:{
			ret.stat = CALC_FAILED;
			return ret;
		}
		
	}	
}



void free_syntax_tree(struct node_t * top) {
	if(!top)
		return;
	
	free_syntax_tree(top->left);
	free_syntax_tree(top->right);
	free(top);
}
