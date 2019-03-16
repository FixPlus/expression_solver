#include "problem_st2.h"

FILE* sp_out;

struct var_table_t solve_expressions(const char* exprs);
enum parse_stat parse_statement(struct lexem_t **lex, struct var_table_t table); 
void skip_to_next_rcurv(struct lexem_t **lex);
enum parse_stat parse_block(struct lexem_t **lex, struct var_table_t table);
void go_to_semicolon(struct lexem_t** lex);
int is_lcurv_brace(struct lexem_t lex);
int is_rcurv_brace(struct lexem_t lex);
int is_else(struct lexem_t lex);
void skip_statement(struct lexem_t** lex);
struct calc_data_t parse_condition(struct lexem_t **lex, struct var_table_t table);
enum parse_stat parse_while(struct lexem_t **lex, struct var_table_t table);
enum parse_stat parse_if(struct lexem_t **lex, struct var_table_t table);
enum parse_stat parse_print(struct lexem_t **lex, struct var_table_t table);
enum parse_stat parse_out(struct lexem_t **lex, struct var_table_t table);
enum parse_stat parse_assignation(struct lexem_t **lex, struct var_table_t table);
int is_valid_expression(struct lexem_t* start,struct lexem_t* stopsign); 