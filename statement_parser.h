#pragma once
#include "problem_st2.h"


struct node_t* parse_code(const char* exprs);
struct node_t* parse_statement(struct lexem_t **lex); 
void skip_to_next_rcurv(struct lexem_t **lex);
struct node_t* parse_block(struct lexem_t **lex);
void go_to_semicolon(struct lexem_t** lex);
int is_lcurv_brace(struct lexem_t lex);
int is_rcurv_brace(struct lexem_t lex);
int is_else(struct lexem_t lex);
struct node_t* parse_condition(struct lexem_t **lex);
struct node_t* parse_while(struct lexem_t **lex);
struct node_t* parse_if(struct lexem_t **lex);
struct node_t* parse_print(struct lexem_t **lex);
struct node_t* parse_out(struct lexem_t **lex);
struct node_t* parse_assignation(struct lexem_t **lex);
int is_valid_expression(struct lexem_t* start,struct lexem_t* stopsign); 
struct node_t* create_empty_node();
struct node_t* create_var_node(struct lexem_t lex);