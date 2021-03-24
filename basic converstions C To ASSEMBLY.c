#include "CodeGenerator.h"

//firas ismail 28/11/2020

//i rather use my own flags
#define FALSE 0;
#define TRUE 1;

//enum for the curent types
typedef enum
{
	myINTEGER,
	myDOUBLE,
	myFLOAT,
	NONE
}

TYPE;

//where are we in the opertaion
typedef enum
{
	LEFT,
	RIGHT
}//did not use it as i expected 

SIDE;
//used like off on switch to not print when using -AST
typedef enum
{
	NO,
	YES
}

PRINT;
//by address or by value or niether if needed "STAM" type
typedef enum
{
	ADDRESS,
	VALUE,
	STAM,
	BOTH //added this in situations like X+=5
}

HOW;

int isItNew = TRUE;		  //is the Var new to the symTable
char *tmpName = NULL;	  //ordinary temp
TYPE tmpType = myINTEGER; //ordinary temp of type TYPE

typedef struct variable
{
	int Size; //not sure if needed
	int Address;
	TYPE Type;
	char *Name;
	int Scope;
	struct variable *Next;
	struct variable *Prev;
	/*Think! what does a Variable contain? */
} * Variable;

typedef struct symbol_table
{
	Variable Tail;
	Variable Head;
	/*Think! what does a symbol_table contain? */

} * Symbol_table;

// ordinary list functionss
Variable newGroom(char *Name, TYPE Type, int Scope);
void addSymbol(Symbol_table Head, Variable Symbol);
int isItThere(Symbol_table Head, char *Name);
void deleteTable(Variable Head);
//some counters to keep track of the numbering process of
// the jump tags in the code
int whileCounter = 0;
int ifCounter = 0;
int conditionCounter = 0;
int base = 5; //starts at base of our course
int ifElseCounter = 0;
int doWhileCounter = 0;
int forCounter = 0;
int Scope = 1; //know the scope that we are at
			   //starts from one but could be anything
			   //could add the feature to "turn off" the scope virables before exiting the block //TODO
/*
 *	You need to build a data structure for the symbol table
 *	I recommend to use linked list.
 *
 *	You also need to build some functions that add/remove/find element in the symbol table
 */

/*
 *	This recursive function is the main method for Code Generation
 *	Input: treenode (AST)
 *	Output: prints the Pcode on the console
 */
Symbol_table mySymbolTable = NULL;
int wraper(treenode *root, SIDE Side, HOW How, PRINT Print)//could make shorter, would not touch at this point
{
	// if(!root)return SUCCESS ;
	//	printf("warpaer called\n");
	int Temp;
	if_node *ifn;
	for_node *forn;
	leafnode *leaf;
	if (!root)
		return SUCCESS;
	switch (root->hdr.which)
	{
	case LEAF_T:
		leaf = (leafnode *)root;
		switch (leaf->hdr.type)
		{
		case TN_LABEL:
			/*Maybe you will use it later */
			break;
		case TN_IDENT:
			// printf("entered the TN_IDENT\n");
			/*variable case */
			/*
					 *	In order to get the identifier name you have to use:
					 *	leaf->data.sval->str
					 */
			// printf("entered the TN_IDENT\n");
			if (isItNew)
			{

				tmpName = (char *)malloc(sizeof(char) * strlen(leaf->data.sval->str));

				strcpy(tmpName, leaf->data.sval->str);
				/*if (!isItThere(mySymbolTable, tmpName))*/ {
					addSymbol(mySymbolTable, newGroom(tmpName, tmpType, Scope));
					//printf("created the id %s and the type %d with address %d IN scope %d \n",tmpName,tmpType,base-1,Scope);
				}
			}
			if (How == ADDRESS || How == BOTH)
			{
				if (Print == YES)
					printf("ldc %d\n", isItThere(mySymbolTable, leaf->data.sval->str));
			}
			if ((How == VALUE || How == BOTH) && Print == YES)
			{
				printf("ldc %d\n", isItThere(mySymbolTable, leaf->data.sval->str));
				printf("ind\n");
			}
			Side = RIGHT;
			break;

		case TN_COMMENT:
			/*Maybe you will use it later */
			break;

		case TN_ELLIPSIS:
			/*Maybe you will use it later */
			break;

		case TN_STRING:
			/*Maybe you will use it later */
			break;

		case TN_TYPE:
			if (isItNew == YES)
			{
				if (leaf->hdr.tok == FLOAT)
				{
					tmpType = myFLOAT;
					//  printf("assign type myFLOAT\n");
				}
				else if (leaf->hdr.tok == INT)
				{
					tmpType = myINTEGER;
					// printf("assign type myINTEGER\n");
				}
				else if (leaf->hdr.tok == DOUBLE)
				{
					tmpType = myDOUBLE;
					//printf("assign type myINTEGER\n");
				}
				else
				{
					isItNew = NO;
				}
			}
			/*Maybe you will use it later */
			break;

		case TN_INT:
			/*Constant case */
			/*
					 *	In order to get the int value you have to use: 
					 *	leaf->data.ival 
					 */

			//  printf("entered the TN_INT\n");
			if (Print == YES) //printf("entered the TN_REAL\n");
				printf("ldc %d\n", leaf->data.dval);
			break;

		case TN_REAL:
			/*Constant case */
			/*
					 *	In order to get the real value you have to use:
					 *	leaf->data.dval
					 */
			//    printf("entered the TN_REAL\n");
			if (Print == YES) //printf("entered the TN_REAL\n");
				printf("ldc %.2f\n", leaf->data.dval);
			break;
		}

		break;

	case IF_T:
		//   printf("entered the IF_T\n");
		ifn = (if_node *)root;
		switch (ifn->hdr.type)
		{
		case TN_IF:
			//     printf("entered the TN_IF\n");
			if (ifn->else_n == NULL)
			{
				/*if case (without else)*/
				Temp = ifCounter++;
				How = VALUE;
				
				wraper(ifn->cond, Side, How, Print); //code_recur(ifn->cond);
													 //	Side = LEFT;
				if (Print == YES)
					printf("fjp if_end%d\n", Temp);
				How = STAM;
				wraper(ifn->then_n, Side, How, Print); //code_recur(ifn->then_n);
				if (Print == YES)
					printf("if_end%d:\n", Temp);
				/*How = STAM; was here rememberrrrrrrrrTODO*/
			}
			else
			{
				/*if - else case*/
				//printf("entered the if-else\n");
				Temp = ifElseCounter++;
				How = VALUE;
								wraper(ifn->cond, Side, How, Print); //code_recur(ifn->cond);
													 //		Side = LEFT;
				if (Print == YES)
					printf("fjp ifelse_else%d\n", Temp);
				wraper(ifn->then_n, Side, How, Print); //code_recur(ifn->then_n);
				if (Print == YES)
				{
					printf("ujp ifelse_end%d\n", Temp);
					printf("ifelse_else%d:\n", Temp);
				}
				wraper(ifn->else_n, Side, How, Print); //code_recur(ifn->else_n);
				if (Print == YES)
					printf("ifelse_end%d:\n", Temp);
				How = STAM;
			}
			break;

		case TN_COND_EXPR:
			//     printf("entered the (cond)?(exp):(exp)\n");
			/*(cond)?(exp):(exp); */
			Temp = ifElseCounter++;
			How = VALUE;
			///	Side = RIGHT;
			wraper(ifn->cond, Side, How, Print); //code_recur(ifn->cond);
												 //		Side = LEFT;
			if (Print == YES)
				printf("fjp cond_else%d\n", Temp);
			wraper(ifn->then_n, Side, How, Print); //	code_recur(ifn->then_n);
			if (Print == YES)
			{
				printf("ujp condLabel_end%d\n", Temp);
				printf("cond_else%d:\n", Temp);
			}
			How = VALUE;
			wraper(ifn->else_n, Side, How, Print); //	code_recur(ifn->else_n);
			if (Print == YES)
				printf("condLabel_end%d:\n", Temp);
			How = STAM;
			break;

		default:
			/*Maybe you will use it later */
			wraper(ifn->cond, Side, How, Print);   //code_recur(ifn->cond);
			wraper(ifn->then_n, Side, How, Print); //	code_recur(ifn->then_n);
			wraper(ifn->else_n, Side, How, Print); //	code_recur(ifn->else_n);
		}

		break;

	case FOR_T:
		forn = (for_node *)root;
		switch (forn->hdr.type)
		{
		case TN_FUNC_DEF:
			/*Function definition */
			/*e.g. int main(...) { ... } */
			/*Look at the output AST structure! */
			wraper(forn->init, Side, How, Print);	//code_recur(forn->init);
			wraper(forn->test, Side, How, Print);	//code_recur(forn->test);
			wraper(forn->incr, Side, How, Print);	//code_recur(forn->incr);
			wraper(forn->stemnt, Side, How, Print); //code_recur(forn->stemnt);
			break;

		case TN_FOR:
			/*For case*/
			/*e.g. for(i=0;i < 5;i++) { ... } */
			/*Look at the output AST structure! */
			Temp = forCounter++;
			// printf("entered the TN_FOR %d\n",Temp);
			wraper(forn->init, Side, How, Print); //code_recur(forn->init);
			if (Print == YES)
				printf("for_loop%d:\n", Temp);
			wraper(forn->test, Side, How, Print); //code_recur(forn->test);
			if (Print == YES)
				printf("fjp for_end%d\n", Temp);
			How = VALUE;
			wraper(forn->stemnt, Side, How, Print); //code_recur(forn->stemnt);
			wraper(forn->incr, Side, How, Print);	//code_recur(forn->incr);
			if (Print == YES)
			{
				printf("ujp for_loop%d\n", Temp);
				printf("for_end%d:\n", Temp);
			}

			break;
		default:
			/*Maybe you will use it later */
			wraper(forn->init, Side, How, Print);	//code_recur(forn->init);
			wraper(forn->test, Side, How, Print);	//code_recur(forn->test);
			wraper(forn->stemnt, Side, How, Print); //code_recur(forn->stemnt);
			wraper(forn->incr, Side, How, Print);	//code_recur(forn->incr);code_recur(forn->init);
			wraper(forn->test, Side, How, Print);	//code_recur(forn->test);
			wraper(forn->stemnt, Side, How, Print); //code_recur(forn->stemnt);
			wraper(forn->incr, Side, How, Print);	//code_recur(forn->incr);
		}

		break;

	case NODE_T:
		switch (root->hdr.type)
		{
		case TN_PARBLOCK:
			/*Maybe you will use it later */

			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_PARBLOCK_EMPTY:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_TRANS_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_FUNC_DECL:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_FUNC_CALL:
			/*Function call */
			if (strcmp(((leafnode *)root->lnode)->data.sval->str, "printf") == 0)
			{
				/*printf case */
				/*The expression that you need to print is located in */
				/*the currentNode->right->right sub tree */
				/*Look at the output AST structure! */
				How = VALUE;
				
				wraper(root->rnode->rnode, Side, How, Print); //code_recur(root->lnode);
				Side = NONE;
				
				if (Print == YES)
					printf("print\n");
				break;
			}
			else
			{
				/*other function calls - for HW3 */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}

			if (strcmp(((leafnode *)root->lnode)->data.sval->str, "return") == 0)
				break;

		case TN_BLOCK: // printf("{\n");
					   /*Maybe you will use it later */
			Scope++;
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
												   //   printf("}\n");
			Scope--;
			break;

		case TN_ARRAY_DECL:
			/*array declaration - for HW2 */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_EXPR_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_NAME_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_ENUM_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_FIELD_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_PARAM_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_IDENT_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_TYPE_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_COMP_DECL:
			/*struct component declaration - for HW2 */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_DECL:
			// printf("enterd TN_DECL\n");
			if (!mySymbolTable)
			{
				// printf("first time\n");
				mySymbolTable = (Symbol_table)malloc(sizeof(struct symbol_table));
				mySymbolTable->Head = NULL;
				mySymbolTable->Tail = NULL;
			}
			isItNew = YES;
			if (tmpName)
			{
				free(tmpName);
				tmpName = NULL;
			}
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			/*if(!isItThere(mySymbolTable,tmpName))*/
			//addSymbol(mySymbolTable, newGroom(tmpName, tmpType));
			// printf("created the id %s and the type %d with address %d \n",tmpName,tmpType,base-1);
			if (tmpName)
				free(tmpName);
			tmpName = NULL;
			tmpType = NONE;
			isItNew = NO;
			break;

		case TN_DECL_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_DECLS:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_STEMNT_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_STEMNT:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_BIT_FIELD:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_PNTR:
			/*pointer - for HW2! */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_TYPE_NME:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_INIT_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_INIT_BLK:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_OBJ_DEF:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_OBJ_REF:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_CAST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_JUMP:
			if (root->hdr.tok == RETURN)
			{
				/*return jump - for HW2! */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}
			else if (root->hdr.tok == BREAK)
			{
				/*break jump - for HW2! */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}
			else if (root->hdr.tok == GOTO)
			{
				/*GOTO jump - for HW2! */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}

			break;

		case TN_SWITCH:
			/*Switch case - for HW2! */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_INDEX:
			/*call for array - for HW2! */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_DEREF:
			/*pointer derefrence - for HW2! */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_SELECT:
			/*struct case - for HW2! */
			if (root->hdr.tok == ARROW)
			{
				/*Struct select case "->" */
				/*e.g. struct_variable->x; */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}
			else
			{
				/*Struct select case "." */
				/*e.g. struct_variable.x; */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}

			break;

		case TN_ASSIGN:
			if (root->hdr.tok == EQ)
			{
				/*Regular assignment "=" */
				/*e.g. x = 5; */
				How = ADDRESS;						   //need the address of X
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				How = VALUE;
				
				wraper(root->rnode, Side, How, Print);
				tmpType = NONE;
				
				if (Print == YES)
					printf("sto\n");
				How = STAM;
			}
			else if (root->hdr.tok == PLUS_EQ)
			{
				/*Plus equal assignment "+=" */
				/*e.g. x += 5; */
				How = BOTH;
				wraper(root->lnode, Side, How, Print);
				//		code_recur(root->lnode);
				
				How = VALUE;
				wraper(root->rnode, Side, How, Print); //		code_recur(root->rnode);

				
				if (Print == YES)
				{
					printf("add\n");
					printf("sto\n");
				}
			}
			else if (root->hdr.tok == MINUS_EQ)
			{
				/*Minus equal assignment "-=" */
				/*e.g. x -= 5; */

				How = BOTH;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				How = VALUE;
				
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //				Side = LEFT;
				if (Print == YES)
				{
					printf("sub\n");
					printf("sto\n");
				}
				How = STAM;
			}
			else if (root->hdr.tok == STAR_EQ)
			{
				/*Multiply equal assignment "*=" */
				/*e.g. x *= 5; */
				How = BOTH;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				How = VALUE;
				
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //				Side = LEFT;
				if (Print == YES)
				{
					printf("mul\n");
					printf("sto\n");
				}

				How = STAM;
			}
			else if (root->hdr.tok == DIV_EQ)
			{
				/*Divide equal assignment "/=" */
				/*e.g. x /= 5; */
				How = BOTH;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				How = VALUE;
				
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //				Side = LEFT;
				if (Print == YES)
				{
					printf("div\n");
					printf("sto\n");
				}
				How = STAM;
			}

			break;

		case TN_EXPR:
			switch (root->hdr.tok)
			{
			case CASE:
				/*you should not get here */
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
				break;

			case INCR:

				if (!root->lnode)
				{
					if (How == ADDRESS)
					{
						How = ADDRESS;
						wraper(root->rnode, Side, How, Print);
						How = VALUE;
						wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("inc 1\n");
							printf("sto\n");
						}
					}
					else
					{

						How = ADDRESS;
						wraper(root->rnode, Side, How, Print);
						How = VALUE;
						wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("inc 1\n");
							printf("sto\n");
						}
						wraper(root->rnode, Side, How, Print);
					}

					//  Side=RIGHT;
					break;
					//  wraper(root->rnode, Side, How, Print);
				}
				else
				{
					if (How == ADDRESS)
					{
						How = ADDRESS;
						wraper(root->lnode, Side, How, Print);
						How = VALUE;
						wraper(root->lnode, Side, How, Print);
						//wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("inc 1\n");
							printf("sto\n");
						}
					}
					else
					{
						wraper(root->lnode, Side, How, Print);
						How = ADDRESS;
						wraper(root->lnode, Side, How, Print);
						How = VALUE;
						wraper(root->lnode, Side, How, Print);
						//wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("inc 1\n");
							printf("sto\n");
						}

						//How = VALUE;
					}
				}
				// Side=LEFT;

				//wraper(root->lnode, Side, How, Print);	//code_recur(root->lnode);
				//wraper(root->rnode, Side, How, Print);

				//		How = ADDRESS;
				//		if (Side = RIGHT)
				//		{
				//			wraper(root->rnode, Side, How, Print);
				//		}
				//		How = STAM;
				break;

			case DECR:
				/*Decrement token "--" */
				if (!root->lnode)
				{
					if (How == ADDRESS)
					{
						How = ADDRESS;
						wraper(root->rnode, Side, How, Print);
						How = VALUE;
						wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("dec 1\n");
							printf("sto\n");
						}
						//Side = RIGHT;
					}
					else
					{
						How = ADDRESS;
						wraper(root->rnode, Side, How, Print);
						How = VALUE;
						wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("dec 1\n");
							printf("sto\n");
						}
						wraper(root->rnode, Side, How, Print);
					}
					break;
					//  wraper(root->rnode, Side, How, Print);
				}
				else
				{
					if (How == ADDRESS)
					{
						How = ADDRESS;

						wraper(root->lnode, Side, How, Print);
						How = VALUE;
						wraper(root->lnode, Side, How, Print);

						//wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("dec 1\n");
							printf("sto\n");
						}
					}
					else
					{
						wraper(root->lnode, Side, How, Print);
						How = ADDRESS;

						wraper(root->lnode, Side, How, Print);
						How = VALUE;
						wraper(root->lnode, Side, How, Print);

						//wraper(root->rnode, Side, How, Print);
						if (Print = YES)
						{
							printf("dec 1\n");
							printf("sto\n");
						}
					}
				}
				break;

			case PLUS:
				/*Plus token "+" */
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("add\n");
				How = STAM;
				break;

			case MINUS:
				/*Minus token "-" */
				if (!root->lnode)
				{
					How = VALUE;
					wraper(root->rnode, Side, How, Print); //	 code_recur(root->rnode);
					if (Print == YES)
						printf("neg\n");
					How = STAM;
				}
				else
				{
					How = VALUE;
					////		Side = RIGHT;
					wraper(root->lnode, Side, How, Print); //  code_recur(root->lnode);
					wraper(root->rnode, Side, How, Print); //  code_recur(root->rnode);
														   //		Side = LEFT;
					if (Print == YES)
						printf("sub\n");
					How = STAM;
				}

				break;

			case DIV:
				/*Divide token "/" */
				How = VALUE;
				//		Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //		Side = LEFT;
				if (Print == YES)
					printf("div\n");
				How = ADDRESS;
				break;

			case STAR:
				/*multiply token "*" */
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				How = VALUE;
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("mul\n");
				//	How = ADDRESS;
				break;

			case AND:
				/*And token "&&" */
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("and\n");
				How = ADDRESS;
				break;

			case OR:
				/*Or token "||" */
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("or\n");

				break;

			case NOT:
				/*Not token "!" */
				How = VALUE;
				//			Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //		Side = LEFT;
				if (Print == YES)
					printf("not\n");

				break;

			case GRTR:
				/*Greater token ">" */
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //		Side = LEFT;
				if (Print == YES)
					printf("grt\n");

				break;

			case LESS:
				/*Less token "<" */
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //		Side = LEFT;
				if (Print == YES)
					printf("les\n");

				break;

			case EQUAL:
				/*Equal token "==" */
				How = VALUE;
				//		Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //			Side = LEFT;
				if (Print == YES)
					printf("equ\n");

				break;

			case NOT_EQ:
				/*Not equal token "!=" */
				How = VALUE;
				//			Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //			Side = LEFT;
				if (Print == YES)
				{
					printf("neq\n");
					//  printf("not\n");
				}


				break;

			case LESS_EQ:
				/*Less or equal token "<=" */
				How = VALUE;
				//				Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //				Side = LEFT;
				if (Print == YES)
					printf("leq\n");
			
				break;

			case GRTR_EQ:
				/*Greater or equal token ">=" */
				How = VALUE;
				//				Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //						Side = LEFT;
				if (Print == YES)
					printf("geq\n");
				How = ADDRESS;
				break;

			default:
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
				break;
			}

			break;

		case TN_WHILE:
			/*While case */
			Temp = whileCounter++;
			if (Print == YES)
				printf("while_loop%d:\n", Temp);
			How = VALUE;
			//			Side = RIGHT;
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			//	wraper(root->rnode,Side,How,Print);//code_recur(root->rnode);
			//	code_recur(root->lnode);
			//			Side = LEFT;
			How = ADDRESS;
			if (Print == YES)
			{
				printf("fjp while_end%d\n", Temp);
			}
			wraper(root->rnode, Side, How, Print);
			//printf("gets here\n");
			if (Print == YES)
			{
				printf("ujp while_loop%d\n", Temp);
				printf("while_end%d:\n", Temp);
			}
			break;

		case TN_DOWHILE:
			/*Do-While case */
			Temp = doWhileCounter++;
			if (Print == YES)
				printf("do_while%d:\n", Temp);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);	code_recur(root->rnode);
			How = VALUE;
			//		Side = RIGHT;
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			How = ADDRESS;
			//	Side = LEFT;
			if (Print == YES)
			{
				printf("fjp end_do%d\n", Temp);
				printf("ujp do_while%d\n", Temp);
				printf("end_do%d:\n", Temp);
			}
			break;

		case TN_LABEL:
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		default:
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
		}

		break;

	case NONE_T:
		printf("Error: Unknown node type!\n");
		exit(FAILURE);
	}
	//printf("prints success\n");
	return SUCCESS;
}

int code_recur(treenode *root)
{
	return wraper(root, LEFT, STAM, YES); //same funtion
										  //Side: represents the side of the operation/ son side
										  //How: is by value or address to load values
										  //Print: is like on//off button for symbolTable create but not
										  //       create the pcode
}

/*
 *	This function prints all the variables on your symbol table with their data
 *	Input: treenode (AST)
 *	Output: prints the Sumbol Table on the console
 */
void print_symbol_table(treenode *root)
{
	Variable i; /////////changethe table
	char type[100] = {0};
	printf("---------------------------------------\n");
	printf("Showing the Symbol Table:\n");

	wraper(root, LEFT, STAM, NO);
	// printf("gets here\n");
	for (i = mySymbolTable->Head; i != NULL; i = i->Next)
	{
		//printf("%d\n", i->Type);
		switch (i->Type)
		{
		case 2:
			//printf("gets here 3 \n");
			strcpy(type, "float");
			break;
		case 0:
			//printf("gets here 1 \n");
			strcpy(type, "int"); //printf("gets here 1 \n");
			break;
		case 1:
			//printf("gets here 2 \n");
			strcpy(type, "double");
			break;
		}
		printf("Name: %s		Address: %d		Type: %s\n", i->Name, i->Address, type);
	}
	if (mySymbolTable)
		deleteTable(mySymbolTable->Head);
	if (mySymbolTable)
		free(mySymbolTable);
}

typedef enum
{
	intSize = 4,
	floatSize = 4,
	charSize = 1,
	realSize = 4

} SIZE;

/*

int getSize(TYPE Type){
  switch (Type)
  {
 :
    
    break;
  
  default:
    break;
  }
}
*/
Variable newGroom(char *Name, TYPE Type, int Scope)
{
	// printf("trying to create a symbol, %s , %d\n",Name,Type);
	Variable Groom = (Variable)malloc(sizeof(struct variable));
	if (!Groom)
		return NULL; //check
	Groom->Name = (char *)malloc(sizeof(strlen(Name) * sizeof(char)));
	Groom->Next = NULL;
	Groom->Prev = NULL;
	strcpy(Groom->Name, Name);
	Groom->Scope = Scope;
	//printf(" the scope created %d\n ",Scope);
	Groom->Size = /*getSize(Type)*/ 4; ///do i really need it ??
	Groom->Address = base++;
	Groom->Type = Type;

	return Groom;
}

void addSymbol(Symbol_table Table, Variable Symbol)
{
	if (!Table)
		return;
	if (Table->Head == NULL)
	{
		Table->Head = Symbol;
		Table->Tail = Symbol;
		Symbol->Prev = Table->Tail;
	}
	else
	{
		Table->Tail->Next = Symbol;
		Symbol->Prev = Table->Tail;
		Table->Tail = Symbol;
	}
}

int isItThere(Symbol_table Table, char *Name)
{
	if (!Table)
		return 0;
	int lastAddress = 0;
	Variable i, temp;
	for (i = Table->Head; i; i = i->Next)
	{
		//	printf("trying %s in scope %d current Scope %d\n",i->Name,i->Scope,Scope);
		if (strcmp(Name, i->Name) == 0 && i->Scope <= Scope) //
		{
			//printf("fo  und %s\n",Name);
			temp = i;
			tmpType = i->Type;
			lastAddress = i->Address;
			//printf("found: %s, in scope %d, the last address found %d, scope of the var %d \n",Name,Scope,lastAddress,i->Scope);
			// return lastAddress;
		} //for scope
	}
	//printf("filed to find match %s in scope %d current Scope %d\n", /*i->Name*/"VAR" ,  i->Scope , Scope);

	return lastAddress;
}

void deleteTable(Variable Head)
{
	if (!Head)
		return;
	deleteTable(Head->Next);
	if (Head->Name)
		free(Head->Name);
	if (Head)
		free(Head);
}