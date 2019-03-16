#include "statement_parser.h"



struct var_table_t solve_expressions(const char* exprs){
	struct var_table_t ret = create_var_table(TABLE_SIZE);  //creating an array of variables
	sp_out = stdout;
	struct lex_array_t lexarr = lex_string(&exprs);      //Reading expression (chars -> lexems)	
	if(!lexarr.lexems){
		return ret;
	}
	struct lexem_t* current = lexarr.lexems;
	while(!is_end(*current)){   //doing this till END lexem 
		if(parse_block(&current, ret) == PARSE_FAILED){
			printf("Procces failed, abandonig further calculations\n");
			return ret;
		}
	}
	free_lexarray(lexarr);
	
	if(sp_out != stdout){
		fclose(sp_out);
		sp_out = stdout;
	}
	
	return ret;
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

enum parse_stat parse_block(struct lexem_t **lex, struct var_table_t table){
	if(is_lcurv_brace(**lex)){
		*lex = *lex + 1;
		while(!is_rcurv_brace(**lex)){
			if((**lex).kind == END)
				return PARSE_FAILED;
			if(parse_statement(lex, table) == PARSE_FAILED)
				return PARSE_FAILED;
		}
		*lex = *lex + 1;
		return PARSE_SUCCES;
	}
	else
		return parse_statement(lex, table);
	
}

struct calc_data_t parse_and_calc_expression(struct lexem_t **lex, struct var_table_t table){
	struct calc_data_t ret = {CALC_FAILED, 0};
	
	if(is_end(**lex)) return ret;
	struct node_t* root = build_syntax_tree(lex, table);    
	if(!root) return ret;
			
	ret = calculate(root);	
	return ret;
}

// condition ::= (expression)

struct calc_data_t parse_condition(struct lexem_t **lex, struct var_table_t table){
	struct calc_data_t ret = {CALC_FAILED, 0};
	if(!is_brace(**lex)){
		printf("Expected '(', found: '"); print_lexem(**lex); printf("'\n");
		return ret;
	}
	
	
	struct lexem_t* enter_point = *lex;
	skip_to_next_rbrac(lex);
	*lex = *lex - 1;
	
	if(!is_valid_expression(enter_point, *lex))
		return ret;
	
	if(!is_brace(**lex)){
		printf("Missing ')'\n");
		return ret;
	}
	*lex = *lex - 1;
	
	return parse_and_calc_expression(lex, table);
}

// while :: = while condition [*true* block\statement]

enum parse_stat parse_while(struct lexem_t **lex, struct var_table_t table){
	struct lexem_t* enter_point = *lex;
		
	struct calc_data_t condition = parse_condition(lex, table);
	if(condition.stat == CALC_FAILED)
		return PARSE_FAILED;
	
	while(condition.result){
		*lex = *lex + 1;
		enum parse_stat stat = parse_block(lex, table);
		if(stat == PARSE_FAILED)
			return PARSE_FAILED;
		*lex = enter_point;
		condition = parse_condition(lex, table);
		
		if(condition.stat == CALC_FAILED)
			return PARSE_FAILED;
	}
	*lex = *lex + 1;
	skip_statement(lex);
	return PARSE_SUCCES;
}


// if :: = < if condition [*true* block\statement] > + < else [*false* block\statement] > (false block is optional)

enum parse_stat parse_if(struct lexem_t **lex, struct var_table_t table){

	struct calc_data_t condition = parse_condition(lex, table);
	if(condition.stat == CALC_FAILED)
		return PARSE_FAILED;

	*lex = *lex + 1;

	if(condition.result){
		if(parse_block(lex, table) == PARSE_FAILED)
			return PARSE_FAILED;
		
		if(is_else(**lex))
			skip_statement(lex);
		
		
		return PARSE_SUCCES;	
	}
	else{
		skip_statement(lex);
		if(is_else(**lex)){
			*lex = *lex + 1;
			if(parse_block(lex, table) == PARSE_FAILED)
				return PARSE_FAILED;
		}
		return PARSE_SUCCES;
	}

}

// print :: = print var/num/all;

enum parse_stat parse_print(struct lexem_t **lex, struct var_table_t table){
	switch((**lex).kind){
		case VAR:{
			struct var_t var = get_var(table, (**lex).lex.name);
			if(is_undef(var)){
				throw_the_undef_var_exception((**lex).lex.name);
				return PARSE_FAILED;
			}
			print_var(sp_out, var);
			break;
		}
		case NUM:{
			printf("%d; ", (**lex).lex.num);
			break;
			
		}
		case KEYWORD:{
			switch((**lex).lex.word){
				case ALL:{
					print_vars(sp_out, table);
					break;
				}
				default:{
					printf("Unexpected keyword '"); print_lexem(**lex); printf("' after 'print'\n");
					break;
				}
			}
			break;
		}
		default:{
			printf("Expected variable or num after 'print'\n");
			return PARSE_FAILED;
		}
	}
	*lex = *lex + 1;
	if(!is_semicolon(**lex)){
		printf("Missing ';' after 'out' expression\n");
		return PARSE_FAILED;
	}
	*lex = *lex + 1;
	return PARSE_SUCCES;
}

// out :: = out var; (var used just to contain name of the file)

enum parse_stat parse_out(struct lexem_t **lex, struct var_table_t table){
	switch((**lex).kind){
		case VAR:{
			sp_out = fopen((**lex).lex.name,"w");
			if(!sp_out){
				sp_out = stdout;
				printf("Can't open file '"); printf("%s", (**lex).lex.name); printf("'\n");
			}
			break;
		}
		default:{
			printf("Expected name of the file after 'out'\n");
			return PARSE_FAILED;
		}
	}
	*lex = *lex + 1;
	if(!is_semicolon(**lex)){
		printf("Missing ';' after 'out' expression\n");
		return PARSE_FAILED;
	}
	*lex = *lex + 1;
	return PARSE_SUCCES;
}

//assignation :: = VAR = expression;

enum parse_stat parse_assignation(struct lexem_t **lex, struct var_table_t table){
	char* name = (**lex).lex.name;
	
	*lex = *lex + 1;
	
	if(!is_assign(**lex)){
		printf("Expected '=' after '%s', found: '", name);
		print_lexem(**lex);
		printf("'\n");
		return PARSE_FAILED;
	}
	
	struct lexem_t* enter_point = *lex + 1;
	go_to_semicolon(lex);
	if(!is_valid_expression(enter_point, *lex))
		return PARSE_FAILED;
	
	*lex = *lex - 1;
	struct calc_data_t result = parse_and_calc_expression(lex, table);
	*lex = *lex + 1;
	if(result.stat == CALC_FAILED) return PARSE_FAILED;
	
	assign_a_var(table, name, result.result);
	return PARSE_SUCCES;

}

// statement :: = if | while | assignation | print | block | END

enum parse_stat parse_statement(struct lexem_t **lex, struct var_table_t table){
	
	switch((**lex).kind){
		case VAR:{
			return parse_assignation(lex, table);
		}
		case KEYWORD:{
			switch((**lex).lex.word){
				case WHILE:{
					*lex = *lex + 1;
					return parse_while(lex, table);
				}
				case IF:{
					*lex = *lex + 1;
					return parse_if(lex, table);
				}
				case PRINT:{
					*lex = *lex + 1;
					return parse_print(lex, table);
				}
				case OUT:{
					*lex = *lex + 1;
					return parse_out(lex, table);
				}
				default:{ 
					printf("Unexpected keyword: ");
					print_lexem(**lex);
					printf("\n");
					return PARSE_FAILED;
				}
			}
		}
		case BRACE:{
			switch((**lex).lex.b){
				case LCURV:{
					return parse_block(lex, table);
				}
				default:{
					printf("Unexpected brace: ");
					print_lexem(**lex);
					printf("\n");
					return PARSE_FAILED;
				}
			}
		}
		case END: return PARSE_SUCCES;
		default: {
			printf("Unexpected lexem: ");
			print_lexem(**lex);
			printf("\n");
			return PARSE_FAILED;
		}
	}	
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

void skip_statement(struct lexem_t** lex){
	switch((**lex).kind){
		case KEYWORD:{
			switch((**lex).lex.word){
				case IF:{
					*lex = *lex + 1;             //   skips construction: if condition { ... } | if condition statement; 
					skip_to_next_rbrac(lex);
					skip_statement(lex);
					break;
				}
				case WHILE:{                          // skips construction: while condition {...} | while condition statement;
					*lex = *lex + 1;
					skip_to_next_rbrac(lex);
					skip_statement(lex);
					break;				
				}
				case PRINT:{                          // skips: print variable;
					go_to_semicolon(lex);
					*lex = *lex + 1;
					break;				
				}
				case ELSE:{							//skips: else {...} | else statement;
					*lex = *lex + 1;
					skip_statement(lex);
					break;
				}
			}
			break;
		}
		case BRACE:{
			switch((**lex).lex.b){
				case LCURV:{                        //skips {...}
					skip_to_next_rcurv(lex);
					break;
				}
				case LBRAC:{                        //skips (...)
					skip_to_next_rbrac(lex);    
					break;
				}
				default: return;
			}
			break;
		}
		default:{
			go_to_semicolon(lex);                  //skips  statement;
			*lex = *lex + 1;
		}			
	}
}

void go_to_semicolon(struct lexem_t** lex){
	while(!is_semicolon(**lex) && !is_end(**lex)){
		//print_lexem(**lex);
		*lex = *lex + 1;
	}
}	
