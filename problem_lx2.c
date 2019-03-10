#include "problem_lx2.h"

struct lex_array_t lex_string(const char **str) {   //after reaching SEMICOLON it return lexems and moving *str to the next char after it
  assert(str != NULL && *str != NULL);
  
  struct lex_array_t larr = { (struct lexem_t*)calloc(ICAP, sizeof(struct lexem_t)), 
                              0, 
                              ICAP };
  assert(larr.lexems != NULL);
  
  // TODO:
  // for each char in str
  //   if char reminds space, skip it
  //   if char reminds number, lex number
  //   if char reminds brace, lex brace
  //   if char reminds operation, lex operation
  //   otherwise free lex array, return NULL
  
  printf("Reading expression...\n");
  
  int length = strlen(*str);
  
  for(int i = 0;i < length; i++){
	char c = *(*str+i);
	printf("%c",c);
	if(larr.size == larr.capacity - 1){
		larr.capacity *= 2;
		struct lexem_t* new_lexems = (struct lexem_t*)realloc(larr.lexems,larr.capacity * sizeof(struct lexem_t));
		if(!new_lexems){
			free(larr.lexems);
			return larr;
		}
		larr.lexems = new_lexems;
	}
		
	switch(c){
		case '(':{
			larr.lexems[larr.size].kind = BRACE;
			larr.lexems[larr.size].lex.b = LBRAC;
			break;
		}
		case ')':{
			larr.lexems[larr.size].kind = BRACE;
			larr.lexems[larr.size].lex.b = RBRAC;	
			break;			
		}
		case '+':{
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = ADD;	
			break;
		}
		case '-':{
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = SUB;	
			break;
		}
		case '*':{
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = MUL;	
			break;
		}
		case '/':{
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = DIV;
			break;
		}
		case ' ':{
			larr.size--;
			break;
		}
		case '=':{
			larr.lexems[larr.size].kind = ASSIGN;
			break;
		}
		case ';':{
			larr.lexems[larr.size].kind = SEMICOLON;
			larr.size++;
			*str += i + 1;
			return larr;
		}
		case '\n':{
			larr.size--;
			break;
		}

		default:{
			if(isdigit(*(*str+i))){
				larr.lexems[larr.size].kind = NUM;
				larr.lexems[larr.size].lex.num = (c - '0');
				i++;
				while(isdigit(*(*str+i))){
					c =  *(*str+i);
					printf("%c", c);
					larr.lexems[larr.size].lex.num *=10;
					larr.lexems[larr.size].lex.num += (c - '0');
					i++;
				}
				i--;
				break;
			}
			else{
				larr.lexems[larr.size].kind = VAR;
				int cap = 10;
				int length = 0;
				larr.lexems[larr.size].lex.name = (char*)calloc(cap, sizeof(char));
				
				c =  *(*str+i);
				larr.lexems[larr.size].lex.name[length] = c;
				length++;
				i++;
				
				while(!is_std_op_char(*(*str + i))){
					c =  *(*str+i);
					printf("%c", c);
					larr.lexems[larr.size].lex.name[length] = c;
					length++;
					i++;
					if(length == cap){
						cap *= 2;
						larr.lexems[larr.size].lex.name = (char*)realloc(larr.lexems[larr.size].lex.name ,cap);
					}
				}
				i--;
			}
		}
		
	}
	larr.size++;

	  
  }
  
  larr.size++;
  larr.capacity++;
  larr.lexems = (struct lexem_t*)realloc(larr.lexems,larr.capacity * sizeof(struct lexem_t));
  larr.lexems[larr.size-1].kind = END;
  
  *str = NULL;    //reaching the end of string we set str as NULL to tell that we ran out of expressions
  
  printf("\nDone!\n");
  return larr;
}

int is_std_op_char(char c){
	switch(c){
		case '\n': break;
		case ' ': break;
		case '/': break;
		case '*': break;
		case '+': break;
		case '-': break;
		case ')': break;
		case '(': break;
		case ';': break;
		default: return 0;
	}
	return 1;
}

static void
print_op(enum operation_t opcode) {
  switch(opcode) {
    case ADD: printf(" PLUS"); break;
    case SUB: printf(" MINUS"); break;
    case MUL: printf(" MUL"); break;
    case DIV: printf(" DIV"); break;
    default: assert(0 && "unknown opcode");
  }
}

static void
print_brace(enum braces_t bracetype) {
  switch(bracetype) {
    case LBRAC: printf(" LBRAC"); break;
    case RBRAC: printf(" RBRAC"); break;
    default: assert(0 && "unknown bracket");
  }
}

static void
print_num(int n) {
  printf(" NUMBER:%d", n);
}

static void
print_semicolon() {
  printf(" SEMICOLON");
}

static void
print_assign() {
  printf(" ASSIGN");
}

static void
print_variable(char* name) {
  printf(" VAR:%s", name);
}

void
print_lexem(struct lexem_t lxm) {
  switch(lxm.kind) {
    case OP: print_op(lxm.lex.op); break;
    case BRACE: print_brace(lxm.lex.b); break;
    case NUM: print_num(lxm.lex.num); break;
	case END: printf(" END");break;
	case SEMICOLON: print_semicolon(); break;
	case VAR: print_variable(lxm.lex.name); break;
	case ASSIGN: print_assign(); break;
    default: assert(0 && "unknown lexem");
  }
}

void dump_lexarray(struct lex_array_t pl) {
  int i;
  assert(pl.lexems != NULL);
  for (i = 0; i < pl.size; ++i)
    print_lexem(pl.lexems[i]);
}

void free_lexarray(struct lex_array_t lexarr){
	for(int i = 0; i < lexarr.size; i++)
		if(lexarr.lexems[i].kind == VAR)
			free(lexarr.lexems[i].lex.name);
	
	free(lexarr.lexems);
}
