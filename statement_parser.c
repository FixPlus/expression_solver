#include "statement_parser.h"



struct node_t* parse_code(const char* exprs){	
	struct lex_array_t lexarr = lex_string(&exprs);      //Reading expression (chars -> lexems)	
	if(!lexarr.lexems){
		return NULL;
	}
	printf("\nParsing...\n");
	
	struct node_t* right, *left, *node;
	struct lexem_t* current = lexarr.lexems;
	left = parse_block(&current);
	if(!left){
		printf("Parse failed!\n");
		return NULL;
	}
	
	node = left;
	
	while(!is_end(*current)){   //doing this till END lexem 
		node = create_empty_node();
		node->right = parse_block(&current);
		if(!node->right){
			free_syntax_tree(node);
			printf("Parse failed!\n");
			return NULL;
		}
		node->left = left;
		left = node;
	}
	
	free_lexarray(lexarr);
	printf("Parsed!\n");		
	return node;
}

void skip_to_next_rcurv(struct lexem_t **lex){
	int count = 1;
	*lex = *lex + 1;
	while(count > 0){
		if((**lex).kind == BRACE)
			switch((**lex).lex.b){
				case RCURV: count--; break;
				case LCURV: count++; break;
				default: break;
			}
		if((**lex).kind == END)
			return;
		*lex = *lex + 1;
	}
}

void skip_to_next_rbrac(struct lexem_t **lex){
	int count = 1;
	*lex = *lex + 1;
	while(count > 0){
		if((**lex).kind == BRACE)
			switch((**lex).lex.b){
				case RBRAC: count--; break;
				case LBRAC: count++; break;
				default: break;
			}
		if((**lex).kind == END)
			return;
		*lex = *lex + 1;
	}
}

//block ::= { statement; statement; ...} | statement

struct node_t* parse_block(struct lexem_t **lex){
	struct node_t* ret = create_empty_node();
	if(is_lcurv_brace(**lex)){
		*lex = *lex + 1;
		while(!is_rcurv_brace(**lex)){
			if((**lex).kind == END){
				free_syntax_tree(ret);
				return NULL;
			}
			struct node_t* right = parse_statement(lex);
			if(!right){
				free_syntax_tree(ret);
				return NULL;
			}
			struct node_t* new_node = create_empty_node();
			new_node->left = ret;
			new_node->right = right;
			ret = new_node;
		}
		*lex = *lex + 1;
		return ret;
	}
	else
		return parse_statement(lex);
	
}

// condition ::= (expression)

struct node_t* parse_condition(struct lexem_t **lex){
	if(!is_brace(**lex)){
		printf("Expected '(', found: '"); print_lexem(**lex); printf("'\n");
		return NULL;
	}
	
	
	struct lexem_t* enter_point = *lex;
	skip_to_next_rbrac(lex);
	*lex = *lex - 1;
	
	if(!is_valid_expression(enter_point, *lex))
		return NULL;
	
	if(!is_brace(**lex)){
		printf("Missing ')'\n");
		return NULL;
	}
	*lex = *lex - 1;
	return build_syntax_tree(lex);
}

// while :: = while condition [*true* block\statement]

struct node_t* parse_while(struct lexem_t **lex){
	/*
		****WHILE NODE***
		->left node contains condition expression
		->right node contains block of statements
	
	*/	
	struct node_t* left = parse_condition(lex);
	if(!left)
		return NULL;
	
	*lex = *lex + 1;
	struct node_t* right = parse_block(lex);
	if(!right){
		free_syntax_tree(left);
		return NULL;
	}
	
	struct node_t* ret = create_empty_node();
	ret->data.k = NODE_KEYWORD;
	ret->data.u.word = WHILE;
	ret->left = left;
	ret->right = right;
	
	return ret;
}


// if :: = < if condition [*true* block\statement] > + < else [*false* block\statement] > (false block is optional)

struct node_t* parse_if(struct lexem_t **lex){
	/*
		*******IF NODE*****
		->left node contains condition expression
		->right one lead to the "true block" with the ->right node and to the "false block" with ->left
	
	*/
	struct node_t* left = parse_condition(lex);
	if(!left)
		return NULL;

	*lex = *lex + 1;
	struct node_t* right = create_empty_node();
	struct node_t* right_right = parse_block(lex);
	struct node_t* right_left;
	
	if(is_else(**lex)){
		*lex = *lex + 1;
		right_left = parse_block(lex);
	}
	else
		right_left = create_empty_node();
	
	right->left = right_left;
	right->right = right_right;
	if(!right_right || !right_left){
		free_syntax_tree(left);
		free_syntax_tree(right);
		return NULL;
	}
	struct node_t* ret = create_empty_node();
	ret->data.k = NODE_KEYWORD;
	ret->data.u.word = IF;
	ret->left = left;
	ret->right = right;
	return ret;
}

// print :: = print var/num/all;

struct node_t* parse_print(struct lexem_t **lex){
	/*
		*****PRINT NODE*****
		->left node contaions info to print
		
	*/
	
	struct node_t* ret = create_empty_node();
	ret->data.k = NODE_KEYWORD;
	ret->data.u.word = PRINT;
	struct node_t* left = create_empty_node();
	ret->left = left;
	
	switch((**lex).kind){
		case VAR:{
			free(left);
			left = create_var_node(**lex);
			ret->left = left;
			break;
		}
		case NUM:{
			left->data.k = NODE_VAL;
			left->data.u.d = (**lex).lex.num;
			break;
			
		}
		case KEYWORD:{
			switch((**lex).lex.word){
				case ALL:{
					left->data.k = NODE_KEYWORD;
					left->data.u.word = ALL;
					break;
				}
				default:{
					printf("Unexpected keyword '"); print_lexem(**lex); printf("' after 'print'\n");
					free_syntax_tree(ret);
					return NULL;
				}
			}
			break;
		}
		default:{
			printf("Expected variable or num after 'print'\n");
			free_syntax_tree(ret);
			return NULL;
		}
	}
	*lex = *lex + 1;
	if(!is_semicolon(**lex)){
		printf("Missing ';' after 'print' expression\n");
		free_syntax_tree(ret);
		return NULL;
	}
	*lex = *lex + 1;
	return ret;
}

// out :: = out var; (var used just to contain name of the file)

struct node_t* parse_out(struct lexem_t **lex){
	/*
		*****OUT NODE*****
		->left node contains name of the file
	
	*/
	
	
	struct node_t* ret = create_empty_node();
	ret->data.k = NODE_KEYWORD;
	ret->data.u.word = OUT;
	struct node_t* left = create_var_node(**lex);
	if(!left){
		printf("FUCK\n");
		free(ret);
		return NULL;
	}
	ret->left = left;
	
	*lex = *lex + 1;
	if(!is_semicolon(**lex)){
		printf("Missing ';' after 'out' expression\n");
		free_syntax_tree(ret);
		return NULL;
	}
	*lex = *lex + 1;
	return ret;
}

//assignation :: = VAR = expression;

struct node_t* parse_assignation(struct lexem_t **lex){
	/*
		****ASSIGNATION NODE*****
		->left node contains name of the variable;
		->right node contains the expression that will be assigned to this variable
	
	*/
	
	
	struct node_t* left = create_var_node(**lex);
	*lex = *lex + 1;
	
	if(!is_assign(**lex)){
		printf("Expected '=' after '%s', found: '", left->data.u.name);
		print_lexem(**lex);
		printf("'\n");
		free_syntax_tree(left);
		return NULL;
	}
	
	struct lexem_t* enter_point = *lex + 1;
	go_to_semicolon(lex);
	if(!is_valid_expression(enter_point, *lex)){
		free_syntax_tree(left);
		return NULL;
	}
	
	*lex = *lex - 1;
	if(is_end(**lex)){ 
		free_syntax_tree(left);
		return NULL;
	}
	struct node_t* right = build_syntax_tree(lex);    
	if(!right){
		free_syntax_tree(left);
		return NULL;	
	}
	
	struct node_t* ret = create_empty_node();
	ret->data.k = NODE_ASSIGN;
	ret->left = left;
	ret->right = right;
	*lex = *lex + 1;
	return ret;
}

// statement :: = if | while | assignation | print | block | END | semicolon

struct node_t* parse_statement(struct lexem_t **lex){
	
	switch((**lex).kind){
		case VAR:{
			return parse_assignation(lex);
		}
		case KEYWORD:{
			switch((**lex).lex.word){
				case WHILE:{
					*lex = *lex + 1;
					return parse_while(lex);
				}
				case IF:{
					*lex = *lex + 1;
					return parse_if(lex);
				}
				case PRINT:{
					*lex = *lex + 1;
					return parse_print(lex);
				}
				case OUT:{
					*lex = *lex + 1;
					return parse_out(lex);
				}
				default:{ 
					printf("Unexpected keyword: ");
					print_lexem(**lex);
					printf("\n");
					return NULL;
				}
			}
		}
		case BRACE:{
			switch((**lex).lex.b){
				case LCURV:{
					return parse_block(lex);
				}
				default:{
					printf("Unexpected brace: ");
					print_lexem(**lex);
					printf("\n");
					return NULL;
				}
			}
		}
		case END:{
			return create_empty_node();
		}
		default: {
			printf("Unexpected lexem: ");
			print_lexem(**lex);
			printf("\n");
			return NULL;
		}
	}	
}

struct node_t* create_empty_node(){
	/*
		****EMPTY NODE****
		->left node contains statement that should be done first
		->right node contains statement taht should be done after ->left
	*/
	struct node_t* empty = (struct node_t*)calloc(1, sizeof(struct node_t));
	empty->data.k = NODE_NONE;
	return empty;	
}

struct node_t* create_var_node(struct lexem_t lex){
	if(lex.kind != VAR){
		return NULL;
	}
	struct node_t* ret = create_empty_node();
	ret->data.k = NODE_VAR;
	ret->data.u.name = (char*)calloc(1, sizeof(char));
	ret->data.u.name = strcpy(ret->data.u.name, lex.lex.name);
	return ret;
}

int is_else(struct lexem_t lex){
	return (lex.kind == KEYWORD && lex.lex.word == ELSE) ? 1:0;
}

int is_lcurv_brace(struct lexem_t lex){
	return (lex.kind == BRACE && lex.lex.b == LCURV) ? 1:0;
}

int is_rcurv_brace(struct lexem_t lex){
	return (lex.kind == BRACE && lex.lex.b == RCURV) ? 1:0;
}

int is_keyword(struct lexem_t lex){
	return (lex.kind == KEYWORD) ? 1:0;
}

int is_valid_expression(struct lexem_t* start,struct lexem_t* stopsign){    // check if the lexems between 'start' and 'stopsign' satisfy dry expression (without any statement operands and keywords)
	if(!stopsign)
		return 0;
	
	while(start != stopsign){
		if(!is_lcurv_brace(*start) && !is_lcurv_brace(*start) && !is_assign(*start)
								   && !is_semicolon(*start) && !is_keyword(*start)){
			start = start + 1;
			continue;
		}
		else{
			printf("Unexpected '"); print_lexem(*start); printf("' in the middle of expression\n");
			return 0;
		}
	}
	return 1;
}

void go_to_semicolon(struct lexem_t** lex){
	while(!is_semicolon(**lex) && !is_end(**lex)){
		//print_lexem(**lex);
		*lex = *lex + 1;
	}
}	
