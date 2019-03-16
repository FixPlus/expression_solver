#pragma once

#include "problem_lx2.h"
#include "var_table.h"

enum {TABLE_SIZE = 100};
enum node_kind_t { NODE_OP, NODE_VAL};
enum calc_stat_t {CALC_SUCCES, CALC_FAILED};
enum parse_stat {PARSE_SUCCES, PARSE_FAILED};

struct calc_data_t{
	enum calc_stat_t stat;
	int result;
};

struct node_data_t {
  enum node_kind_t k;
  union {
    enum operation_t op;
    int d;
  } u;
};

struct node_t {
  struct node_t *left, *right;
  struct node_data_t data;
};




struct node_t *build_syntax_tree(struct lexem_t** lex, struct var_table_t table);



struct node_t* parse_logic(struct lexem_t** lex, struct var_table_t table);
struct node_t* parse_comparation(struct lexem_t** lex, struct var_table_t table);
struct node_t* parse_expr(struct lexem_t** lex, struct var_table_t table);
struct node_t* parse_mult(struct lexem_t** lex, struct var_table_t table);
struct node_t* parse_term(struct lexem_t** lex, struct var_table_t table);

struct node_t* create_operation_expression(enum operation_t opcode);


int check_end(struct lexem_t* lex);
int is_compare(struct lexem_t lex);
int is_logic(struct lexem_t lex);
int is_plusminus(struct lexem_t lex);
int is_muldiv(struct lexem_t lex);
int is_brace(struct lexem_t lex);
int is_assign(struct lexem_t lex);
int is_var(struct lexem_t lex);
int is_end(struct lexem_t lex);
int is_semicolon(struct lexem_t lex);



struct calc_data_t calculate(struct node_t *top);
int calc_result(struct node_t *top);

void free_syntax_tree(struct node_t * top);
