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
		case '{':{
			larr.lexems[larr.size].kind = BRACE;
			larr.lexems[larr.size].lex.b = LCURV;
			break;
		}
		case '}':{
			larr.lexems[larr.size].kind = BRACE;
			larr.lexems[larr.size].lex.b = RCURV;	
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
		case '\t':{
			larr.size--;
			break;
		}
		case '=':{
			if(*(*str+i + 1) == '='){
				printf("%c", '=');
				larr.lexems[larr.size].kind = OP;
				larr.lexems[larr.size].lex.op = EQUAL;
				i++;
				break;
			}
			larr.lexems[larr.size].kind = ASSIGN;
			break;
		}
		case '>':{
			larr.lexems[larr.size].kind = OP;
			if(*(*str + i + 1) == '='){
				printf("%c", '=');
				larr.lexems[larr.size].lex.op = EQ_OR_MORE;
				i++;
			}
			else
				larr.lexems[larr.size].lex.op = MORE;
			break;
		}
		case '<':{
			larr.lexems[larr.size].kind = OP;
			if(*(*str + i + 1) == '='){
				printf("%c", '=');
				larr.lexems[larr.size].lex.op = EQ_OR_LESS;
				i++;
			}
			else
				larr.lexems[larr.size].lex.op = LESS;
			break;
		}
		case '&':{
			if(*(*str + i + 1) != '&'){
				printf("\nError with '&&' sign\n");
				free_lexarray(larr);
				larr.lexems = NULL;
				larr.size = 0;
				return larr;
			}
			i++;
			printf("%c", '&');
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = LG_AND; 
			break;
		}
		case '|':{
			if(*(*str + i + 1) != '|'){
				printf("\nError with '||' sign\n");
				free_lexarray(larr);
				larr.lexems = NULL;
				larr.size = 0;
				return larr;
			}
			i++;
			printf("%c", '|');
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = LG_OR;
			break;
		}
		case '!':{
			if(*(*str + i + 1) != '='){
				printf("\nError with '!=' sign\n");
				free_lexarray(larr);
				larr.lexems = NULL;
				larr.size = 0;
				return larr;
			}
			i++;
			printf("%c", '=');
			larr.lexems[larr.size].kind = OP;
			larr.lexems[larr.size].lex.op = NOT_EQUAL;
			break;
		}
		case ';':{
			larr.lexems[larr.size].kind = SEMICOLON;
			break;
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
				larr.lexems[larr.size].kind = KEYWORD;
				switch(is_key_word(larr.lexems[larr.size].lex.name)){
					case 1:{
						larr.lexems[larr.size].lex.word = WHILE;
						break;
					}
					case 2:{
						larr.lexems[larr.size].lex.word = IF;
						break;
					}
					case 3:{
						larr.lexems[larr.size].lex.word = PRINT;
						break;
					}
					case 4:{
						larr.lexems[larr.size].lex.word = ELSE;
						break;
					}
					case 5:{
						larr.lexems[larr.size].lex.word = ALL;
						break;
					}
					case 6:{
						larr.lexems[larr.size].lex.word = OUT;
						break;
					}
					default:{
						larr.lexems[larr.size].kind = VAR;
						break;
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

int is_key_word(const char* word){
	if(!strcmp("while", word))
		return 1;
	if(!strcmp("if", word))
		return 2;
	if(!strcmp("print", word))
		return 3;
	if(!strcmp("else", word))
		return 4;
	if(!strcmp("all", word))
		return 5;
	if(!strcmp("out", word))
		return 6;
	return 0;
}
int is_std_op_char(char c){
	switch(c){
		case '\n': break;
		case ' ': break;
		case '/': break;
		case '*': break;
		case '+': break;
		case '-': break;
		case '=': break;
		case ')': break;
		case '(': break;
		case '{': break;
		case '}': break;
		case ';': break;
		default: return 0;
	}
	return 1;
}

static void
print_op(enum operation_t opcode) {
  switch(opcode) {
    case ADD: printf("+"); break;
    case SUB: printf("-"); break;
    case MUL: printf("*"); break;
    case DIV: printf("/"); break;
    case EQUAL: printf("=="); break;
    case NOT_EQUAL: printf("!="); break;
	case EQ_OR_MORE: printf(">="); break;
	case EQ_OR_LESS: printf("<="); break;
	case MORE: printf(">"); break;
	case LESS: printf("<"); break;
	case LG_AND: printf("&&"); break;
	case LG_OR: printf("||"); break;
    default: assert(0 && "unknown opcode");
  }
}

static void
print_brace(enum braces_t bracetype) {
  switch(bracetype) {
    case LBRAC: printf("("); break;
    case RBRAC: printf(")"); break;
    case LCURV: printf("{"); break;
    case RCURV: printf("}"); break;
    default: assert(0 && "unknown bracket");
  }
}

static void
print_num(int n) {
  printf(" %d", n);
}

static void
print_semicolon() {
  printf(";");
}

static void
print_assign() {
  printf("=");
}

static void
print_variable(char* name) {
  printf("%s", name);
}


static void
print_keyword(enum keyword_t word) {
  switch(word) {
    case WHILE: printf("while"); break;
    case IF: printf("if"); break;
	case PRINT: printf("print"); break;
	case ELSE: printf("else"); break;
}
}

void
print_lexem(struct lexem_t lxm) {
  switch(lxm.kind) {
    case OP: print_op(lxm.lex.op); break;
    case BRACE: print_brace(lxm.lex.b); break;
    case NUM: print_num(lxm.lex.num); break;
	case END: printf(" **END**");break;
	case SEMICOLON: print_semicolon(); break;
	case VAR: print_variable(lxm.lex.name); break;
	case ASSIGN: print_assign(); break;
	case KEYWORD: print_keyword(lxm.lex.word); break;
    default: assert(0 && "unknown lexem");
  }
}

void dump_lexarray(struct lex_array_t pl) {
  int i;
  assert(pl.lexems != NULL);
  for (i = 0; i < pl.size; ++i)
    print_lexem(pl.lexems[i]);

	printf("\n");
}

void free_lexarray(struct lex_array_t lexarr){
	for(int i = 0; i < lexarr.size; i++)
		if(lexarr.lexems[i].kind == VAR)
			free(lexarr.lexems[i].lex.name);
	
	free(lexarr.lexems);
}
