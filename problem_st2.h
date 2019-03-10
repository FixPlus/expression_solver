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



struct var_table_t solve_expressions(const char* exprs);
enum parse_stat parse_statement(struct lex_array_t lexarr, struct var_table_t table); 


struct node_t *build_syntax_tree(struct lex_array_t lexarr, struct var_table_t table);


struct calc_data_t calculate(struct node_t *top);
struct node_t* parse_expr(struct lex_array_t* lex, struct var_table_t table);

int is_plusminus(struct lexem_t lex);
int is_muldiv(struct lexem_t lex);
int is_brace(struct lexem_t lex);
struct node_t* parse_mult(struct lex_array_t* lex, struct var_table_t table);
struct node_t* parse_term(struct lex_array_t* lex, struct var_table_t table);
int check_end(struct lexem_t* lex);


int calc_result(struct node_t *top);

void free_syntax_tree(struct node_t * top);
