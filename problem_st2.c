#include "problem_st2.h"

// Grammar:
// state ::= var {=} expr ;
// expr ::= mult {+, -} expr | mult
// mult ::= term {*, /} mult | term
// term ::= ( expr ) | number



struct var_table_t solve_expressions(const char* exprs){
	struct var_table_t ret = create_var_table(TABLE_SIZE);  //creating an array of variables
	
	struct lex_array_t lexarr = lex_string(&exprs);      //Reading expression (chars -> lexems)
		
	while(exprs){   //doing this till lex_string() will return NULL 
		if(parse_statement(lexarr, ret) == PARSE_FAILED){
			printf("Procces failed, abandonig further calculations\n");
			return ret;
		}
		free_lexarray(lexarr);
		lexarr = lex_string(&exprs);      //Reading expression (chars -> lexems)
	}
	free_lexarray(lexarr);
	
	return ret;
}


//Expression solving functions

enum parse_stat parse_statement(struct lex_array_t lexarr, struct var_table_t table){
	
	if(lexarr.size < 3 || lexarr.lexems[0].kind != VAR || lexarr.lexems[1].kind != ASSIGN) return PARSE_FAILED;
	
	int count = 0;
	for(int i = 0; i < lexarr.size; i++)
		if(lexarr.lexems[i].kind == ASSIGN)
			count++;
	
	if(count != 1) return PARSE_FAILED;
	
	
	char* name = lexarr.lexems[0].lex.name;
	
	struct node_t* root = build_syntax_tree(lexarr, table);    
	if(!root) return PARSE_FAILED;
	
	struct calc_data_t result = calculate(root);	
	if(result.stat == CALC_FAILED) return PARSE_FAILED;
	
	assign_a_var(table, name, result.result);
	return PARSE_SUCCES;
}

struct node_t *
build_syntax_tree(struct lex_array_t lexarr, struct var_table_t table) {	
	lexarr.capacity = lexarr.size;	
	printf("\nBuilding syntax tree...\nTotal lexems: %d\n",lexarr.capacity - 1);
	lexarr.size -= 2;                  //will use size as a counter, starting from the end of expression
	struct node_t* ret = parse_expr(&lexarr, table);
	
	if(ret)
		printf("Parse succesful!\n");
	else
		printf("Parse failed: there are some syntax mistakes!\n");
	
	return ret;
}

struct node_t* parse_expr(struct lex_array_t *lex, struct var_table_t table){
	struct node_t* left= parse_mult(lex, table);
	if(!left)
		return NULL;
	
	if(is_plusminus(lex->lexems[lex->size])){
		
		enum operation_t opcode = lex->lexems[lex->size].lex.op; 
		
		lex->size--;
		
		struct node_t* right = parse_expr(lex, table);
		
		if(!right){
			free_syntax_tree(left);
			return NULL;
		}
		
		struct node_t* expr = (struct node_t*)calloc(1,sizeof(struct node_t));
		expr->data.k = NODE_OP;
		expr->data.u.op = opcode;		
		
		expr->left = right;
		expr->right = left;
		return expr;
	}
	else
		if(is_brace(lex->lexems[lex->size]) || lex->lexems[lex->size].kind == ASSIGN) //expression should only end with BRACE or ASSIGN lexem
			return left;
		else{
			free_syntax_tree(left);
			return NULL;
		}
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

int is_var(struct lexem_t lex){
	return lex.kind == VAR ? 1:0;
}

struct node_t* parse_mult(struct lex_array_t *lex, struct var_table_t table){
	struct node_t* left= parse_term(lex, table);
	if(!left)
		return NULL;
	
	
	if(is_muldiv(lex->lexems[lex->size])){
		
		enum operation_t opcode = lex->lexems[lex->size].lex.op;
		lex->size--;
		
		struct node_t* right = parse_mult(lex, table);
		
		if(!right){
			free_syntax_tree(left);
			return NULL;
		}
		
		struct node_t* expr = (struct node_t*)calloc(1,sizeof(struct node_t));
		expr->data.k = NODE_OP;
		expr->data.u.op = opcode;
		
		expr->left = right;
		expr->right = left;
		return expr;
	}
	else
		return left;	
}

struct node_t* parse_term(struct lex_array_t *lex, struct var_table_t table){
	
	if(check_end(lex->lexems + lex->size)){     //checking if we unexpectly reach the end of expression (e.g. mistakes in lexems). In case when something goes wrong we safely stop building
		printf("Unexpected reach of the end of expression!\n");
		return NULL;
	}
	
	
	struct node_t* ret = (struct node_t*)calloc(1,sizeof(struct node_t));
	
	
	switch(lex->lexems[lex->size].kind){
		case BRACE:{
			lex->size--;
			free(ret);
			ret = parse_expr(lex, table);
			if(!is_brace(lex->lexems[lex->size])){   //if expression didn't end with brace, stopping the tree building
				free_syntax_tree(ret);
				printf("Braces are not correct!\n");
				return NULL;
			}
			lex->size--;
			return ret;
		}
	
		case NUM: {
			ret->data.k = NODE_VAL;
			ret->data.u.d = lex->lexems[lex->size].lex.num;
			lex->size--;
			return ret;
		}
		case VAR: {
			struct var_t var = get_var(table, lex->lexems[lex->size].lex.name);
			if(is_undef(var)){
				free_syntax_tree(ret);
				printf("Variable %s is undefined!\n", lex->lexems[lex->size].lex.name);
				return NULL;				
			}
			ret->data.k = NODE_VAL;
			ret->data.u.d = var.value;
			lex->size--;
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
	return (lex->kind == END || lex->kind == SEMICOLON || lex->kind == ASSIGN) ? 1 : 0;
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
