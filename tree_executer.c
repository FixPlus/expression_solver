#include "tree_executer.h"

FILE* cur_out; 

enum iter_stat_t iter_none(struct node_t* node, struct var_table_t table){
	return (iterate(node->left, table) == ITER_FAILED || iterate(node->right, table) == ITER_FAILED) ? ITER_FAILED : ITER_SUCCES;
}

enum iter_stat_t iter_while(struct node_t* node, struct var_table_t table){
	struct calc_data_t res = calculate(node->left, table);
	if(res.stat == CALC_FAILED)
		return ITER_FAILED;
	
	while(res.result){
		enum iter_stat_t stat = iterate(node->right, table);
		if(stat == ITER_FAILED)
			return ITER_FAILED;
		
		res = calculate(node->left, table);
		if(res.stat == CALC_FAILED)
			return ITER_FAILED;		
	}
	
	return ITER_SUCCES;
	
}

enum iter_stat_t iter_assign(struct node_t* node, struct var_table_t table){
	struct calc_data_t res = calculate(node->right, table);
	if(res.stat == CALC_FAILED)
		return ITER_FAILED;
	
	if(!node->left || node->left->data.k != NODE_VAR || !node->left->data.u.name)
		return ITER_FAILED;
	
	assign_a_var(table, node->left->data.u.name, res.result);
	return ITER_SUCCES;
}

enum iter_stat_t iter_if(struct node_t* node, struct var_table_t table){
	if(!node->right)
		return ITER_FAILED;
	
	struct calc_data_t res = calculate(node->left, table);
	if(res.stat == CALC_FAILED)
		return ITER_FAILED;
	
	if(res.result)
		return iterate(node->right->right, table);	
	else
		return iterate(node->right->left, table);
}

enum iter_stat_t iter_out(struct node_t* node){
	if(!node->left || node->left->data.k != NODE_VAR)
		return ITER_FAILED;
	
	if(cur_out != stdout)
		fclose(cur_out);
	
	cur_out = fopen(node->left->data.u.name,"w");
	if(!cur_out){
		printf("Could not open file '%s'\n", node->left->data.u.name);
		cur_out = stdout;
	}
	else
		printf("Setting out stream as '%s'\n", node->left->data.u.name);
	return ITER_SUCCES;
}

enum iter_stat_t iter_print(struct node_t* node, struct var_table_t table){
	if(!node->left)
		return ITER_FAILED;
	switch(node->left->data.k){
		case NODE_VAR:{
			struct var_t var = get_var(table, node->left->data.u.name);
			if(is_undef(var))
				return ITER_FAILED;
			print_var(cur_out, var);
			return ITER_SUCCES;
		}
		case NODE_VAL:{
			fprintf(cur_out,"%d; ", node->data.u.d);
			return ITER_SUCCES;
		}
		case NODE_KEYWORD:{
			switch(node->left->data.u.word){
				case ALL:{
					print_vars(cur_out, table);
					return ITER_SUCCES;
				}
				default: return ITER_FAILED;
			}
		}
		default: return ITER_FAILED;
	}
}

enum iter_stat_t iterate(struct node_t* node, struct var_table_t table){
	if(!node){
		return ITER_SUCCES;
	}
	
	switch(node->data.k){
		case NODE_NONE: return iter_none(node, table);
		
		case NODE_KEYWORD:{
			switch(node->data.u.word){
				case IF: return iter_if(node, table);
				case WHILE: return iter_while(node, table);
				case PRINT: return iter_print(node, table);
				case OUT: return iter_out(node);
				default: return ITER_FAILED;
			}
		}
		case NODE_ASSIGN: return iter_assign(node, table);
		default: ITER_FAILED;
		
	}
}


int execute_tree(struct node_t* root){
	
	printf("\nProccesing...\n");
	if(!root)
		return 1;
	
	cur_out = stdout;
	struct var_table_t table = create_var_table(100);
	
	enum iter_stat_t stat = iterate(root, table);
	destroy_var_table(&table);
	if(cur_out != stdout){
		fclose(cur_out);
		cur_out = stdout;		
	}
	int exit_code = (stat == ITER_FAILED) ? 1:0;
	printf("\nProcces finished with exit code %d\n", exit_code);
	return exit_code;
}


