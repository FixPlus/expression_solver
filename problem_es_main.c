#include <stdio.h>
#include "problem_st2.h"

enum { MAXLEN = 1024 };


int main(){
  int res;
  struct var_table_t table;
  char inp[MAXLEN] = {0};
  
  res = scanf("%1023c", inp);
  assert(res == 1);
  
  table = solve_expressions(inp);
  
  FILE* file = fopen("tests/std.ans","w");  
  print_vars(file,table);
  fclose(file);
  destroy_var_table(&table);
  return 0;	
}