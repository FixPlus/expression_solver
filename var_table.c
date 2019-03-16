#include "var_table.h"

struct var_table_t create_var_table(int size){
	struct var_table_t ret = {(struct var_node_t**)calloc(size, sizeof(struct var_node_t*)), size};
	return ret;
}

void throw_the_undef_var_exception(const char* name){
	printf("Variable %s is undefined!\n", name);
}

void destroy_var_table(struct var_table_t* table){
	for(int i = 0; i < table->size; i++){
		struct var_node_t* temp = table->hash[i];
		table->hash[i] = NULL;
		while(temp){
			struct var_node_t* temp2 = temp->next;
			free(temp->var.name);
			free(temp);
			temp = temp2;
		}
	}
	free(table->hash);
	table->hash = NULL;
	table->size = 0;
}

int hash_func(struct var_table_t table,const char* name){
	int n = 0;
	
	for(int i = 0; name[i] != '\0'; i++)
		n += name[i] * (1u << i*2);
	//printf("n = %d\n", n);
	n = ((7*n - 11)%(table.size * table.size - 1))%table.size;
//	printf("Hash of %s is %d\n",name, n);
	return n;
}

struct var_t get_var(struct var_table_t table, const char* name){
	struct var_t ret = { NULL, 0};
	
	if(!table.hash || !name) return ret;
	
	
	int n = hash_func(table, name);
	struct var_node_t* temp = table.hash[n];
	
	while(temp){
		if(!strcmp(temp->var.name, name)){
			return temp->var;
		}
		temp = temp->next;
	}
	
	return ret;
}

void assign_a_var(struct var_table_t table,const char* name, int value){	
	if(!table.hash || !name) return;
	
	int n = hash_func(table, name);
	struct var_node_t* temp = table.hash[n];
	
	if(!temp){
		table.hash[n] = (struct var_node_t*)calloc(1, sizeof(struct var_node_t));
		table.hash[n]->var.name = (char*)calloc(strlen(name) + 1, sizeof(char));
		table.hash[n]->var.name = strcpy(table.hash[n]->var.name, name);
		table.hash[n]->var.value = value;
		return;
	}
	

	if(!strcmp(temp->var.name, name)){
		temp->var.value = value;
		return;
	}

	while(temp->next){
		temp = temp->next;
		if(!strcmp(temp->var.name, name)){
			temp->var.value = value;
			return;
		}
	}
	
	temp->next = (struct var_node_t*)calloc(1, sizeof(struct var_node_t));
	temp = temp->next;
	temp->var.name = (char*)calloc(strlen(name) + 1, sizeof(char));
	strcpy(temp->var.name, name);
	temp->var.value = value;
}

int is_undef(struct var_t var){
	return var.name == NULL ? 1 : 0;
}

void print_var(FILE* out, struct var_t var){
	fprintf(out,"%s = %d; ", var.name, var.value);
}
void print_vars(FILE* out, struct var_table_t table){
	for(int i = 0; i < table.size; i++){
		struct var_node_t* temp = table.hash[i];
		while(temp){
			print_var(out, temp->var);
			temp = temp->next;
		}
	}
	printf("\n");
}
