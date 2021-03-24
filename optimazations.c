#include	"CodeGenerator.h"
/*206144750_209247980*/
/*Static Variables : */
static int Flagg = 0;
static int tempo = 0;
static int temp = 0;
static int last_loop = 0;
static int address = 0;
static int ValOrAdd = 0;
static int SwitchFlag = 0;
static int InitFlag = 0;
static char* current_type ;
static int Nest_Level = 0;
static int ifelse_C = 0;
static int if_C = 1;
static int COND_EXPR_C = 0;
static int swithc_C = 0;
static int case_C = 0;
static int for_C = 0;
static int while_C = 0;
static int dowhile_C = 0;
static int data_size = 5;
static int switchSkip =0 ;
static int tempCase;

typedef struct variable {

	/* Think! what does a Variable contain? */
	char* name;
	char* type;
	int size;
	int address;
	int Nested_level;
} Variable;

typedef struct symbol_table {

	/* Think! what does a symbol_table contain? */
	Variable v;
	struct Symbol_table* next;
} Symbol_table;

/*
*	You need to build a data structure for the symbol table
*	I recommend to use linked list.
*
*	You also need to build some functions that add/remove/find element in the symbol table
*/

Symbol_table* head = NULL;
Symbol_table* tail = NULL;

/*add func for data structure : adding element to the tail */
void add_variable(char* V_name)
{
    Symbol_table* tmp = (Symbol_table*)malloc(sizeof(Symbol_table));
    tmp->v.name = V_name;
    tmp->v.type = current_type;
    tmp->v.size = 1;
    tmp->v.address = data_size;
    tmp->v.Nested_level = Nest_Level;
    if (data_size==5)
    {
        head=tmp;
        head->next = NULL;
    }
    else if(data_size==6)
    {
        tail=tmp;
        head->next = tail;
        tail->next = NULL;
    }
    else
    {
        tail->next = tmp;
        tail = tmp;
        tail->next = NULL;
    }
	data_size++;
}
int find_variable(char* V_name , int nest)
{
	Symbol_table* tmp = head;
	int max_add = 0;
	for (int i = 5; i < data_size; i++)
	{
		if ((strcmp(tmp->v.name, V_name) == 0) && (tmp->v.Nested_level == nest))
		{
			return tmp->v.address;
		}
		else 
		{
			tmp = tmp->next;
		}
	}
	tmp = head;
	for (int i = 5; i < data_size; i++)
	{
		if ((strcmp(tmp->v.name, V_name) == 0) && (tmp->v.Nested_level < nest))
		{
			//max_add=(max_add > tmp->v.address)? max_add: tmp->v.address;
			max_add = tmp->v.address;
			tmp = tmp->next;
		}
		else
		{
			tmp = tmp->next;
		}
	}
	if (max_add != 0)
	{
		return max_add;
	}
	printf("Error: Variable Not Found.\n");
	return 0;
}
void remove_variable(char* V_name)
{
	Symbol_table* h1 = head;
	Symbol_table* h2 = head;
	if (!h1)
		return;
	while (h1)
	{
		if (!strcmp(h1->v.name, V_name))
		{
			free(h1);
			return;
		}
		h2 = h1;
		h1 = h1->next;
	}
	return;
}
/*******************************************************************************************************************/
double calculate_value(treenode *root)
{
	if_node* ifn;
	if (root == NULL)
		return 0;
	if (root->hdr.which != LEAF_T) {
		switch (root->hdr.tok) {
		case PLUS:
			return calculate_value(root->lnode) + calculate_value(root->rnode);
		case MINUS:
			return calculate_value(root->lnode) - calculate_value(root->rnode);
		case DIV:
			return calculate_value(root->lnode) / calculate_value(root->rnode);
		case STAR:
			return calculate_value(root->lnode) * calculate_value(root->rnode);
		case AND:
			return calculate_value(root->lnode) && calculate_value(root->rnode);
		case OR:
			return calculate_value(root->lnode) || calculate_value(root->rnode);
		case GRTR:
			return calculate_value(root->lnode) > calculate_value(root->rnode);
		case LESS:
			return calculate_value(root->lnode) < calculate_value(root->rnode);
		case GRTR_EQ:
			return calculate_value(root->lnode) >= calculate_value(root->rnode);
		case LESS_EQ:
			return calculate_value(root->lnode) <= calculate_value(root->rnode);
		case NOT_EQ:
			return calculate_value(root->lnode) != calculate_value(root->rnode);
		case EQUAL:
			return calculate_value(root->lnode) == calculate_value(root->rnode);
		case NOT:
			return !calculate_value(root->rnode);
		}


		if (root->hdr.type == TN_COND_EXPR)
		{
			ifn = (if_node*)root;
			return calculate_value(ifn->cond) ? calculate_value(ifn->then_n) : calculate_value(ifn->else_n);
		}
	}
	else {
		leafnode *leaf = (leafnode*)root;
		if (root->hdr.type == TN_INT) {
			return leaf->data.ival;
		}
		if (root->hdr.type == TN_REAL)
			return leaf->data.dval;
		else
			return 1;
	}
}

int is_constant(treenode *root)
{

	if (root == NULL || root->hdr.type == TN_INT || root->hdr.type == TN_REAL)
		return 1;
	if (root->hdr.tok == STAR) {
		leafnode *leaf;
		if (root->lnode->hdr.which == LEAF_T && root->lnode->hdr.type != TN_IDENT) {
			leaf = (leafnode*)root->lnode;
			if (root->lnode->hdr.type == TN_INT)
				if (leaf->data.ival == 0)
					return 1;
			if (root->lnode->hdr.type == TN_REAL)
				if (leaf->data.dval == 0)
					return 1;

		}
		if (root->rnode->hdr.which == LEAF_T && root->rnode->hdr.type != TN_IDENT) {
			leaf = (leafnode*)root->rnode;
			if (root->rnode->hdr.type == TN_INT)
				if (leaf->data.ival == 0)
					return 1;
			if (root->rnode->hdr.type == TN_REAL)
				if (leaf->data.dval == 0)
					return 1;

		}
		if (is_constant(root->lnode) == 1 && calculate_value(root->lnode) == 0)
			return 1;
		if (is_constant(root->rnode) == 1 && calculate_value(root->rnode) == 0)
			return 1;


	}
	if (root->hdr.tok == DIV) {
		leafnode *leaf;
		if (root->lnode->hdr.which == LEAF_T && root->lnode->hdr.type != TN_IDENT) {
			leaf = (leafnode*)root->lnode;
			if (root->lnode->hdr.type == TN_INT)
				if (leaf->data.ival == 0)
					return 1;
			if (root->lnode->hdr.type == TN_REAL)
				if (leaf->data.dval == 0)
					return 1;
		}
		if (is_constant(root->lnode) == 1 && calculate_value(root->lnode) == 0)
			return 1;
	}
	if (root->hdr.tok == AND) {
		leafnode *leaf;
		if (root->lnode->hdr.which == LEAF_T && root->lnode->hdr.type != TN_IDENT) {
			leaf = (leafnode*)root->lnode;
			if (root->lnode->hdr.type == TN_INT)
				if (leaf->data.ival == 0)
					return 1;
			if (root->lnode->hdr.type == TN_REAL)
				if (leaf->data.dval == 0)
					return 1;

		}
		if (root->rnode->hdr.which == LEAF_T && root->rnode->hdr.type != TN_IDENT) {
			leaf = (leafnode*)root->rnode;
			if (root->rnode->hdr.type == TN_INT)
				if (leaf->data.ival == 0)
					return 1;
			if (root->rnode->hdr.type == TN_REAL)
				if (leaf->data.dval == 0)
					return 1;

		}
		if (is_constant(root->lnode) == 1 && calculate_value(root->lnode) == 0)
			return 1;
		if (is_constant(root->rnode) == 1 && calculate_value(root->rnode) == 0)
			return 1;


	}
	if (root->hdr.type == TN_IDENT)
	{
		return 0;
	}
	return is_constant(root->lnode) && is_constant(root->rnode);
}
/************************************************************************************************************************/

/*
*	This recursive function is the main method for Code Generation
*	Input: treenode (AST)
*	Output: prints the Pcode on the console
*/
int  code_recur(treenode *root)
{
	if_node  *ifn;
	for_node *forn;
	leafnode *leaf;

    if (!root)
        return SUCCESS;

    switch (root->hdr.which){
		case LEAF_T:
			leaf = (leafnode *) root;
			switch (leaf->hdr.type) {
				case TN_LABEL:
					/* Maybe you will use it later */
					break;

				case TN_IDENT:
					/* variable case */
					/*
					*	In order to get the identifier name you have to use:
					*	leaf->data.sval->str
					*/
					if (InitFlag == 0)
					{
						if (!strcmp(leaf->data.sval->str, "main"))
							break;
						address = find_variable(leaf->data.sval->str, Nest_Level);
						if (address > 4)
						{
							printf("ldc %d\n", address);
							if (ValOrAdd == 1)
							{
								printf("ind\n");
							}
						}
					}
					if (InitFlag ==1)
                    {
						add_variable(leaf->data.sval->str);
                    }
					break;

				case TN_COMMENT:
					/* Maybe you will use it later */
					break;

				case TN_ELLIPSIS:
					/* Maybe you will use it later */
					break;

				case TN_STRING:
					/* Maybe you will use it later */
					break;

				case TN_TYPE:
					/* Maybe you will use it later */
					break;

				case TN_INT:
					/* Constant case */
					/*
					*	In order to get the int value you have to use:
					*	leaf->data.ival
					*/
				if(switchSkip==1){
					tempCase = leaf->data.ival;
					switchSkip=0;
					break;
				}
					printf("ldc %d\n", leaf->data.ival);
					break;

				case TN_REAL:
					/* Constant case */
					/*
					*	In order to get the real value you have to use:
					*	leaf->data.dval
					*/
					printf("ldc %f\n", leaf->data.dval);
					break;
					ValOrAdd = 0;
			}
			break;

		case IF_T:
			ifn = (if_node *) root;
			switch (ifn->hdr.type) {

			case TN_IF:
				if (ifn->else_n == NULL) {
					/* if case (without else)*/
					ValOrAdd = 1;
					temp = is_constant(ifn->cond);
					if (temp == 0) {
						code_recur(ifn->cond);
						ValOrAdd = 0;
						printf("fjp end_if%d\n", if_C++);
						code_recur(ifn->then_n);
						if_C--;
						printf("end_if%d:\n", if_C++);
					}
					else {
						temp = calculate_value(ifn->cond);
						if (temp != 0)
							code_recur(ifn->then_n);
					}
				}
				else {
					/* if - else case*/
					ValOrAdd = 1;
					temp = is_constant(ifn->cond);
					if (temp == 0) {
						code_recur(ifn->cond);
						ValOrAdd = 0;
						printf("fjp ifelse_else%d\n", ifelse_C++);
						code_recur(ifn->then_n);
						ifelse_C--;
						printf("ujp ifelse_end%d\n", ifelse_C);
						printf("ifelse_else%d:\n", ifelse_C++);
						code_recur(ifn->else_n);
						ifelse_C--;
						printf("ifelse_end%d:\n", ifelse_C++);
					}
					else {
						temp = calculate_value(ifn->cond);
						if (temp != 0)
							code_recur(ifn->then_n);
						else
							code_recur(ifn->else_n);
					}
				}
				break;

			case TN_COND_EXPR:
				/* (cond)?(exp):(exp); */
				ValOrAdd = 1;
				temp = is_constant(ifn->cond);
				if (temp == 0) {
					code_recur(ifn->cond);
					//ValOrAdd = 0;
					printf("fjp cond_else%d\n", COND_EXPR_C++);
					code_recur(ifn->then_n);
					COND_EXPR_C--;
					printf("ujp condLabel_end%d\n", COND_EXPR_C);
					printf("cond_else%d:\n", COND_EXPR_C++);
					
					code_recur(ifn->else_n);
					COND_EXPR_C--;
					printf("condLabel_end%d:\n", COND_EXPR_C++);
				}
				else {
					temp = calculate_value(ifn->cond);
					if (temp != 0)
						code_recur(ifn->then_n);
					else
						code_recur(ifn->else_n);
				}
				break;

			default:
				/* Maybe you will use it later */
				code_recur(ifn->cond);
				code_recur(ifn->then_n);
				code_recur(ifn->else_n);
			}
			break;

		case FOR_T:
			forn = (for_node *) root;
			switch (forn->hdr.type) {

			case TN_FUNC_DEF:
				/* Function definition */
				/* e.g. int main(...) { ... } */
				/* Look at the output AST structure! */
				code_recur(forn->init);
				code_recur(forn->test);
				code_recur(forn->incr);
				code_recur(forn->stemnt);
				break;

			case TN_FOR:
				/* For case*/
				/* e.g. for(i=0;i<5;i++) { ... } */
				/* Look at the output AST structure! */
				last_loop = 2;
				code_recur(forn->init);
				printf("for_loop%d:\n", for_C);
				code_recur(forn->test);
				printf("fjp for_end%d\n", for_C++);
				code_recur(forn->stemnt);
				code_recur(forn->incr);
				for_C--;
				printf("ujp for_loop%d\n", for_C);
				printf("for_end%d:\n", for_C++);
				last_loop = 0;
				break;

			default:
				/* Maybe you will use it later */
				code_recur(forn->init);
				code_recur(forn->test);
				code_recur(forn->stemnt);
				code_recur(forn->incr);
			}
			break;

		case NODE_T:
			switch (root->hdr.type) {
				case TN_PARBLOCK:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_PARBLOCK_EMPTY:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_TRANS_LIST:
					/* Maybe you will use it later */
					/* Global variables : */
					InitFlag = 1;
					code_recur(root->lnode);
					code_recur(root->rnode);
					InitFlag = 0;
					break;

				case TN_FUNC_DECL:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_FUNC_CALL:
					/* Function call */
					if (strcmp(((leafnode*)root->lnode)->data.sval->str, "printf") == 0) {
						/* printf case */
						/* The expression that you need to print is located in */
						/* the currentNode->right->right sub tree */
						/* Look at the output AST structure! */
						ValOrAdd = 1;
						code_recur(root->rnode->rnode);
						printf("print\n");
						ValOrAdd = 0;
					}
					else {
						/* other function calls - for HW3 */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					break;

				case TN_BLOCK:
					/* Maybe you will use it later */
					/* Nest_Level shows the block depth level */
					Nest_Level++;
					code_recur(root->lnode);
					code_recur(root->rnode);
					/* In this case we have to delete incompatible variables*/
					/*********************************************IMPORTANT*********************************************/
					Nest_Level--;
					break;

				case TN_ARRAY_DECL:
					/* array declaration - for HW2 */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_EXPR_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_NAME_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_ENUM_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_FIELD_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_PARAM_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_IDENT_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_TYPE_LIST:
					/* Maybe you will use it later */
					if ((root->lnode) != NULL)
					{
						switch (root->lnode->hdr.tok)
						{
						case VOID:
							current_type = "void";
							code_recur(root->lnode);
							code_recur(root->rnode);
							break;
						case INT:
							current_type = "int";
							code_recur(root->lnode);
							code_recur(root->rnode);
							break;

						case DOUBLE:
							current_type = "double";
							code_recur(root->lnode);
							code_recur(root->rnode);
							break;

						case FLOAT:
							current_type = "float";
							code_recur(root->lnode);
							code_recur(root->rnode);
							break;
						}
					}
					break;

				case TN_COMP_DECL:
					/* struct component declaration - for HW2 */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_DECL:
					/* structs declaration - for HW2 */
					InitFlag = 1;
					code_recur(root->lnode);
					code_recur(root->rnode);
					InitFlag = 0;
					break;

				case TN_DECL_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_DECLS:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_STEMNT_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_STEMNT:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_BIT_FIELD:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_PNTR:
					/* pointer - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_TYPE_NME:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_INIT_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_INIT_BLK:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_OBJ_DEF:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_OBJ_REF:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_CAST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_JUMP:
					if (root->hdr.tok == RETURN) {
						/* return jump - for HW2! */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					else if (root->hdr.tok == BREAK) {
						/* break jump - for HW2! */
						switch (last_loop)
						{
						case(1):
							if(switchSkip==2){
								switchSkip=0;
								break;
							}
							printf("ujp switch_end%d\n", swithc_C);
							break;
						case(2):
							printf("ujp for_end%d\n", for_C);
							break;
						case(3):
							printf("ujp while_end%d\n", while_C - 1);
							break;
						case(4):
							printf("ujp Do-While_end%d\n", dowhile_C - 1);
							break;
						}
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					else if (root->hdr.tok == GOTO) {
						/* GOTO jump - for HW2! */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					break;

				case TN_SWITCH:
					/* Switch case - for HW2! */
					last_loop = 1;
					ValOrAdd = 1;
					if (is_constant(root->lnode))
					{
						tempo = calculate_value(root->lnode);
						SwitchFlag = 1;
						//printf("00000000000000000000\n");
						code_recur(root->rnode);
						SwitchFlag = 0;
						//printf("11111111111111111111\n");
						break;
					}
					else
					{
						code_recur(root->lnode);
						printf("switch%d_case%d:\n", swithc_C, case_C++);
						code_recur(root->rnode);
						printf("switch_end%d:\n", swithc_C++);
					}
					case_C = 0;
					last_loop = 0;
					break;

				case TN_INDEX:
					/* call for array - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_DEREF:
					/* pointer derefrence - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_SELECT:
					/* struct case - for HW2! */
					if (root->hdr.tok == ARROW){
						/* Struct select case "->" */
						/* e.g. struct_variable->x; */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					else{
						/* Struct select case "." */
						/* e.g. struct_variable.x; */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					break;

				case TN_ASSIGN:
					if(root->hdr.tok == EQ){
						/* Regular assignment "=" */
						/* e.g. x = 5; */
						if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							/* case X=1 */
						{
							code_recur(root->lnode);
							printf("ldc %f\n", calculate_value(root->rnode));
							printf("sto\n");
							break;
						}
						else
						{
							ValOrAdd = 0;
							code_recur(root->lnode);
							ValOrAdd = 1;
							code_recur(root->rnode);
							ValOrAdd = 0;
							printf("sto\n");
							break;
						}
					}
					else if (root->hdr.tok == PLUS_EQ){
						/* Plus equal assignment "+=" */
						/* e.g. x += 5; */
						if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							/* case X+=1 */
						{
							break;
						}
						else
						{
							code_recur(root->lnode);
							ValOrAdd = 1;
							code_recur(root->lnode);
							code_recur(root->rnode);
							printf("add\n");
							printf("sto\n");
							ValOrAdd = 0;
							break;
						}
					}
					else if (root->hdr.tok == MINUS_EQ){
						/* Minus equal assignment "-=" */
						/* e.g. x -= 5; */
						if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							/* case X-=1 */
						{
							break;
						}
						else
						{
							code_recur(root->lnode);
							ValOrAdd = 1;
							code_recur(root->lnode);
							code_recur(root->rnode);
							printf("sub\n");
							printf("sto\n");
							ValOrAdd = 0;
							break;
						}
					}
					else if (root->hdr.tok == STAR_EQ){
						/* Multiply equal assignment "*=" */
						/* e.g. x *= 5; */
						if (is_constant(root->rnode) && calculate_value(root->rnode) == 1)
							/* case X*=1 */
						{
							break;
						}
						else if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							/* case X*=0 */
						{
							code_recur(root->lnode);
							printf("ldc 0\n");
							printf("sto\n");
							break;
						}
						else
						{
							code_recur(root->lnode);
							ValOrAdd = 1;
							code_recur(root->lnode);
							code_recur(root->rnode);
							printf("mul\n");
							printf("sto\n");
							ValOrAdd = 0;
							break;
						}
					}
					else if (root->hdr.tok == DIV_EQ){
						/* Divide equal assignment "/=" */
						/* e.g. x /= 5; */
						if (is_constant(root->rnode) && calculate_value(root->rnode) == 1)
							/* case X/=1 */
						{
							break;
						}
						else
						{
							code_recur(root->lnode);
							ValOrAdd = 1;
							code_recur(root->lnode);
							code_recur(root->rnode);
							printf("div\n");
							printf("sto\n");
							ValOrAdd = 0;
							break;
						}
					}
					break;

				case TN_EXPR:
					switch (root->hdr.tok) {
					  case CASE:
					      /* you should not get here */
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  break;

					  case INCR:
						  /* Increment token "++" */
						  //code_recur(root->lnode);
						  if (root->lnode)
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  ValOrAdd = 0;
							  code_recur(root->lnode);
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  printf("inc 1\n");
							  printf("sto\n");
						  }
						  //code_recur(root->rnode);
						  if (root->rnode)
						  {
							  ValOrAdd = 0;
							  code_recur(root->rnode);
							  ValOrAdd = 1;
							  code_recur(root->rnode);
							  printf("inc 1\n");
							  printf("sto\n");
							  ValOrAdd = 1;
							  code_recur(root->rnode);
						  }
						  break;

					  case DECR:
						  /* Decrement token "--" */
						  if (root->lnode)
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  ValOrAdd = 0;
							  code_recur(root->lnode);
							  ValOrAdd = 1;
							  printf("dec 1\n");
							  printf("sto\n");
						  }
						  if (root->rnode)
						  {
							  ValOrAdd = 0;
							  code_recur(root->rnode);
							  ValOrAdd = 1;
							  code_recur(root->rnode);
							  printf("dec 1\n");
							  printf("sto\n");
							  ValOrAdd = 1;
							  code_recur(root->rnode);
						  }
						  break;

					  case PLUS:
					  	  /* Plus token "+" */
						  if (is_constant(root))
							  /* case 5+3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode)==0 )
							  /* case 0+X */
						  {
							  code_recur(root->rnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode)==0 )
							  /* case X+0 */
						  {
							  code_recur(root->lnode);
							  break;
						  }
						  else
						  {
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  if(Flagg==0)
								printf("add\n");
							  Flagg = 0;
							  break;
						  }
						  break;

					  case MINUS:
					  	  /* Minus token "-" */
						  if (is_constant(root))
							  /* case 5-3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) == 0)
							  /* case 0-X */
						  {
							  code_recur(root->rnode);
							  printf("neg\n");
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							  /* case X-0 */
						  {
							  code_recur(root->lnode);
							  break;
						  }
						  else
						  {
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  if (root->lnode == NULL)
								  printf("neg\n");
							  else
								  printf("sub\n");
							  break;
						  }
						  break;

					  case DIV:
					  	  /* Divide token "/" */
						  if (is_constant(root))
							  /* case 5/3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) == 0)
							  /* case 0/X */
						  {
							  //code_recur(root->rnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							  /* case X/0 ??? */
						  {
							  //code_recur(root->lnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 1)
							  /* case X/1 */
						  {
							  code_recur(root->lnode);
							  break;
						  }
						  else
						  {
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  printf("div\n");
							  break;
						  }
						  break;

					  case STAR:
					  	  /* multiply token "*" */
						  if (is_constant(root))
							  /* case 5*3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) == 0)
							  /* case 0*X */
						  {
							  //code_recur(root->rnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							  /* case X*0 */
						  {
							  //code_recur(root->lnode);
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) == 1)
							  /* case 1*X */
						  {
							  code_recur(root->rnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 1)
							  /* case X*1 */
						  {
							  code_recur(root->lnode);
							  break;
						  }
						  else
						  {
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  printf("mul\n");
							  break;
						  }
						  break;

					  case AND:
					  	  /* And token "&&" */
						  if (is_constant(root))
							  /* case 5&&3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) == 0)
							  /* case 0&&X */
						  {
							  Flagg = 1;
							  //printf("ldc 0\n");
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							  /* case X&&0 */
						  {
							  //printf("ldc 0\n");
							  Flagg = 1;
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) != 0)
							  /* case 1&&X */
						  {
							  code_recur(root->rnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) != 0)
							  /* case X&&1 */
						  {
							  code_recur(root->lnode);
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("and\n");
							  break;
						  }
						  break;

					  case OR:
					  	  /* Or token "||" */
						  if (is_constant(root))
							  /* case 5||3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) != 0)
							  /* case 0||X */
						  {
							  printf("ldc 1\n");
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) != 0)
							  /* case X||0 */
						  {
							  printf("ldc 1\n");
							  break;
						  }
						  else if (is_constant(root->lnode) && calculate_value(root->lnode) == 0)
							  /* case 1||X */
						  {
							  code_recur(root->rnode);
							  break;
						  }
						  else if (is_constant(root->rnode) && calculate_value(root->rnode) == 0)
							  /* case X||1 */
						  {
							  code_recur(root->lnode);
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("or\n");
							  break;
						  }
						  break;

					  case NOT:
					  	  /* Not token "!" */
						  ValOrAdd = 1;
						  code_recur(root->lnode);
						  ValOrAdd = 1;
						  code_recur(root->rnode);
						  ValOrAdd = 0;
						  printf("not\n");
						  break;

					  case GRTR:
					  	  /* Greater token ">" */
						  if (is_constant(root))
							  /* case 5>3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("grt\n");
							  break;
						  }
						  break;

					  case LESS:
					  	  /* Less token "<" */
						  if (is_constant(root))
							  /* case 5<3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("les\n");
							  break;
						  }
						  break;

					  case EQUAL:
					  	  /* Equal token "==" */
						  if (is_constant(root))
							  /* case 5==3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("equ\n");
							  break;
						  }
						  break;

					  case NOT_EQ:
					  	  /* Not equal token "!=" */
						  ValOrAdd = 1;
						  code_recur(root->lnode);
						  ValOrAdd = 1;
						  code_recur(root->rnode);
						  ValOrAdd = 0;
						  printf("neq\n");
						  break;

					  case LESS_EQ:
					  	  /* Less or equal token "<=" */
						  if (is_constant(root))
							  /* case 5<=3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("leq\n");
							  break;
						  }
						  break;

					  case GRTR_EQ:
					  	  /* Greater or equal token ">=" */
						  if (is_constant(root))
							  /* case 5>=3 */
						  {
							  printf("ldc %f\n", calculate_value(root));
							  break;
						  }
						  else
						  {
							  ValOrAdd = 1;
							  code_recur(root->lnode);
							  code_recur(root->rnode);
							  ValOrAdd = 0;
							  printf("geq\n");
							  break;
						  }
						  break;

					  default:
						code_recur(root->lnode);
						code_recur(root->rnode);
						break;
					}
					break;

				case TN_WHILE:
					/* While case */
					last_loop = 3;
					printf("while_loop%d:\n", while_C++);
					code_recur(root->lnode);
					while_C--;
					printf("fjp while_end%d\n", while_C++);
					code_recur(root->rnode);
					while_C--;
					printf("ujp while_loop%d\n", while_C);
					printf("while_end%d:\n", while_C++);
					last_loop = 0;
					break;

				case TN_DOWHILE:
					/* Do-While case */
					last_loop = 4;
					printf("Do-While_loop%d:\n", dowhile_C++);
					code_recur(root->rnode);
					dowhile_C--;
					printf("fjp Do-While_end%d\n", dowhile_C++);
					code_recur(root->lnode);
					dowhile_C--;
					printf("ujp Do-While_loop%d\n", dowhile_C);
					printf("Do-While_end%d:\n", dowhile_C++);
					last_loop = 4;
					break;

				case TN_LABEL:
					if (SwitchFlag == 1)
					{
						//printf("the left node gives\n");
						switchSkip =1;
						code_recur(root->lnode);
					//	printf("temp saved is %d\n",tempo);
						if (tempo == tempCase)
						{
							switchSkip=2;
							code_recur(root->rnode);
						}
					}
					if (SwitchFlag == 0)
					{
						printf("dpl\n");
						code_recur(root->lnode);
						printf("equ\n");
						printf("fjp switch%d_case%d\n", swithc_C, case_C);
						code_recur(root->rnode);
						printf("switch%d_case%d:\n", swithc_C, case_C++);
					}
					break;

				default:
					code_recur(root->lnode);
					code_recur(root->rnode);
			}
			break;

		case NONE_T:
			printf("Error: Unknown node type!\n");
			exit(FAILURE);
    }

    return SUCCESS;
}


/*
*	This function prints all the variables on your symbol table with their data
*	Input: treenode (AST)
*	Output: prints the Symbol Table on the console
*/
void print_symbol_table(treenode *root) {
	if (code_recur(root))
		return;
	Symbol_table* h = head;
	printf("---------------------------------------\n");
	printf("Showing the Symbol Table:\n");
	/*
	*	add your code here
	*/
	printf("Name  |Type   |Address  |Size   |Nested level\n");
	while (h)
	{
		printf("%s      %s      %d      %d      %d\n", h->v.name, h->v.type, h->v.address, h->v.size, h->v.Nested_level);
		h = h->next;
	}
	printf("---------------------------------------\n");
	free(head);
	return;
}
