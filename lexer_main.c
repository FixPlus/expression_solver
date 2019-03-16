#include <stdio.h>
#include "statement_parser.h"

enum { MAXLEN = 1024 };


int main(){
  int res;
  char inp[MAXLEN] = {0};
  
  res = scanf("%1023c", inp);
  assert(res == 1);
  
  const char* kek = inp;
  struct lex_array_t lex = lex_string(&kek);
  dump_lexarray(lex);
  return 0;	
}