#pragma once

#include "problem_st2.h"

enum iter_stat_t {ITER_SUCCES, ITER_FAILED};

int execute_tree(struct node_t* root);
enum iter_stat_t iterate(struct node_t* node, struct var_table_t table);