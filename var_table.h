#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct var_t {
	char* name;
	int value;
};


struct var_node_t {
	struct var_node_t* next;
	struct var_t var;
};

struct var_table_t {
	struct var_node_t** hash;
	int size;
};




struct var_table_t create_var_table(int size);
void destroy_var_table(struct var_table_t* table);

struct var_t get_var(struct var_table_t table, const char* name);
void assign_a_var(struct var_table_t table,const char* name, int value);

int hash_func(struct var_table_t table,const char* name);
int is_undef(struct var_t var);
void print_vars(FILE* out, struct var_table_t table);
void throw_the_undef_var_exception(const char* name);
void print_var(FILE* out, struct var_t var);