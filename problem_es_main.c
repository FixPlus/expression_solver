#include <stdio.h>
#include "statement_parser.h"
#include "tree_executer.h"

enum { MAXLEN = 1024 };


int main(){
  int res;
  struct var_table_t table;
  char inp[MAXLEN] = {0};
  
  res = scanf("%1023c", inp);
  assert(res == 1);
  
  struct node_t* root = parse_code(inp);
  if(!root)
	  return 0;
  
  execute_tree(root);
  
  free_syntax_tree(root);
  
  return 0;	
}