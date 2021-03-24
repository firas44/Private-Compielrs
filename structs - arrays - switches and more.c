#include "CodeGenerator.h"

#include <math.h>
//firas ismail 21/12/2020 

//i rather use my own flags
#define FALSE 0;
#define TRUE 1;

//enum for the curent types
typedef enum
{
	myINTEGER,
	myDOUBLE,
	myFLOAT,
	NONE,
	myStruct,
	myPointer,
	myArray
}

TYPE;

//where are we in the opertaion
typedef enum
{
	LEFT,
	RIGHT
} //did not use it as i expected

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
	char *label;
	int indexAtLoacArray;
	int increaseAmount;
	int targetAddress;
	char *pointerName;
	char *varName;
	char *arrayname;
	int dimensionSize;
	int *die;
	char * arrayType;


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

int casesCounter = 0; //
int switchCounter = 0;
char *lastBreakLabel[1000]; //a realstic expectaion about the max number of possible
							// BREAKS THAT ARE INSIDE ONE ANOTHER!
int currentBreakIndex = 0;
Symbol_table mySymbolTable = NULL;
char labels;

//strs for saving the the break condition.
char *shorStrTemp = NULL;
char *longStrTemp = NULL;

//strcuts arrays and counters

typedef struct struct_
{
	char *Name;
	Symbol_table thisSymbolTable;
	int size;
} * Struct;

Struct locUsedStructs[1000];

int structCounter = 0;

char *structTempName = NULL;
PRINT skipFlag = NO;
PRINT nowRefrencing = NO;

char *tmpLabel;
PRINT onlyALabel = NO;
PRINT nowSelecting = NO;
int baseInSymbolTable = -1;
char *structIdentifiers[1000];
int structIdentiferIndex = 0;
int tezaherPointer = 0;
PRINT tezaherRefrence = NO;
int lastEnteredIndex = 0;
int checkarr = 0;
int oneArrayCounter = 1;
int labelcounter = 0;
Variable lastBeforeAdding = NULL;

char *tmpRefrence = NULL;
int lastRefrencedAddress = 0;
int lastRefrencedStruct;
//Variable lastRefrencedStruct;
int pointedAddress;

//array stuff here

int arrayEnterdIndexesSize = 0;
//char *arrayEnterdIndexes;

int justDoneRefrencing = 0;

char *lastUsedArray;
//TODO a mili
int arrType = 0;

int arrayEnterdIndexes[10000000];
int localSize = 1;
int arrayRefrencing = 0;
int extraIND = 0;

int printIND = 0;

/*







TN_INDEX









*/

int endedStructIndexes = 0;
int enteredBefore = 0;





char* father;
int  tempixa;

int specialStructIND = 0;
int usedspecialStructIND = 0;

int isALabel(char *suspect)
{
	//printf("function is a label \n ");

	Variable itr = mySymbolTable->Head;
	for (; itr != NULL; itr = itr->Next)
	{ //printf("gets here\n");
		//printf("searching for the label %s against %s\n",suspect,itr->label);
		if (!strcmp(itr->label, suspect))
		{
			//printf("found a label %s its name %s \n", itr->label, itr->Name);
			return 1;
		}
	}

	return 0;
}

Variable isAVariable(char *Name)
{
	//printf("function: isavariable \n");
	Variable itr = mySymbolTable->Head;
	while (itr)
	{
		if (!itr->varName)
		{
			itr = itr->Next;
			continue;
		}

		if (!strcmp(itr->varName, Name))
		{
			return itr;
		}
		itr = itr->Next;
	}

	return NULL;
}

Variable getByAdress(int Address)
{
	//printf("function get by addrress \n ");
	Variable itr = mySymbolTable->Head;
	while (itr)
	{

		if (itr->Address == Address)
		{
			return itr;
		}
		itr = itr->Next;
	}

	return NULL;
}

Variable getByVarName(char *Name)
{
	//printf("function getByVarName\n ");
	Variable itr = mySymbolTable->Head;
	while (itr)
	{
		if (!itr->varName)
		{
			itr = itr->Next;
			continue;
		}
		if (!strcmp(Name, itr->varName))
		{
			return itr;
		}

		itr = itr->Next;
	}

	return NULL;
}
Struct getStructByName(char *Name)
{

	for (int i = 0; i < structCounter; i++)
	{
		//printf("function getStructByName %s %s\n ",locUsedStructs[i]->Name,Name );
		if (locUsedStructs[i]->Name && !strcmp(locUsedStructs[i]->Name, Name))
		{
			return locUsedStructs[i];
		}
	}

	return NULL;
}

void printsymtab()
{
	if (!mySymbolTable)
		return;
	Variable itr = mySymbolTable->Head;
	while (itr)
	{

		itr = itr->Next;
	}

	return;
}

int isAstructComponent(char *strucName, char *Label)
{
	int i = 0;
	for (; i < structCounter; i++)
	{
		//printf("checking %s against %s \n", locUsedStructs[i]->Name, strucName);
		if (!strcmp(locUsedStructs[i]->Name, strucName))
		{
			break;
		}
	}
	Variable temp = locUsedStructs[i]->thisSymbolTable->Head;
	while (temp)
	{
		//printf("checking %s against %s \n", temp->varName, Label);
		if (temp->varName && !strcmp(temp->varName, Label))
		{
			//printf("returned %d \n", temp->Address);
			return temp->Address;
		}

		temp = temp->Next;
	}

	return 0;
}

char *convertNumberIntoArray(int N)
{

	int numOfDigits = log10(N) + 1;
	char *arr = calloc(numOfDigits, sizeof(char));
	for (int i = 0; i < numOfDigits; i++, N /= 10)
	{
		arr[i] = N % 10;
	}
	//printf("%s---------------------------------\n", arr);
	return arr;
}
void ldcArray()
{//	printf("gets hereerrererereeerer\n");

	Variable itr = mySymbolTable->Head;
	while (itr)
	{
		if (!itr->arrayname)
		{

			itr = itr->Next;
			continue;
		}
		if (!strcmp(lastUsedArray, itr->arrayname))
		{
			printf("ldc %d\n", itr->Address);
		
			return;
		}

		itr = itr->Next;
	}

	return;
}
int ixaCounter = 1;
void calculateixa()
{
	//printf("*****************************************\n");

	//printf("here i hould get size of one array  cell%s\n",lastUsedArray);

	//printf("*******************************************\n");

	//printf("the last refrenced array in print ixa is: __ %s\n", lastUsedArray);
	Variable itr = mySymbolTable->Head;
	int tempixa = ixaCounter;
	while (itr)
	{
		if (!strcmp(itr->Name, lastUsedArray) && itr->dimensionSize)
			break;
		itr = itr->Next;
	}
	int flag = 0;

	int *temp = itr->die;
	int localixa = 1;
	int size = (itr->dimensionSize > 7) ? 2 : 1;
	//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^%d\n",size);
	while (temp[++tempixa])
	{
		flag = 1;
		//printf("itr->dimensionSize %d\n", itr->dimensionSize);
		localixa *= temp[tempixa];
	}
	if (flag)
	{
		ixaCounter++;
		printf("ixa %d\n", localixa * size);
		return;
	}
	printf("ixa %d\n dec 0\n", localixa * size);
	endedStructIndexes = 1;
	if (specialStructIND)
	{
		printf("ind\n");
		usedspecialStructIND = 1;
	}

	ixaCounter++;
	return;
}
int wraper(treenode *root, SIDE Side, HOW How, PRINT Print) //could make shorter, would not touch at this point
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
	//		printf("+*+*+*+*+*+*+*+*+*+*+*+*+*+++*++*++*+*+*+++*+++*+* %d\n", checkarr);

			// printf("entered the TN_IDENT\n");
			/*variable case */
			/*
					 *	In order to get the identifier name you have to use:
					 *	leaf->data.sval->str
					 */
			// printf("tezaherRefrence && nowRefrencing: %s\n",leaf->data.sval->str);
			if (arrayRefrencing)
			{
	//			checkarr ? printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n") : 0;

				if (Side == LEFT)
				{
	//				printf("arrayRefrencing\n");
					ixaCounter = 1;
					father=lastUsedArray;
					tempixa=ixaCounter;
					lastUsedArray = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
					strcpy(lastUsedArray, leaf->data.sval->str);
					ldcArray();
					//isItNew=YES;

					arrayRefrencing = 0;
				}
				if (Side == RIGHT)
				{
	//				printf("in i case it should get in here onlt at the i\n");

					Variable itr = mySymbolTable->Head;
					while (itr)
					{
						if (!itr->Name)
						{

							itr = itr->Next;
							continue;
						}
						if (!strcmp(leaf->data.sval->str, itr->Name))
						{
							printf("ldc %d\nind\n", itr->Address);
							
							//printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\narrayEnterdIndexesSize: %d ixa counter %d last %s\n ", arrayEnterdIndexesSize, ixaCounter, lastUsedArray);
							Variable itr = mySymbolTable->Head;
							while (itr)
							{
								if (!itr->arrayname)
								{

									itr = itr->Next;
									continue;
								}
								if (!strcmp(lastUsedArray, itr->arrayname))
								{
									break;
								}

								itr = itr->Next;
							}
							int tempixa = ixaCounter;
							int flag = 0;

							int *temp = itr->die;
							int localixa = 1;
							int size = (itr->dimensionSize > 7) ? 2 : 1;
							//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^%d\n",size);
							while (temp[++tempixa])
							{
								flag = 1;
								//printf("itr->dimensionSize %d\n", itr->dimensionSize);
								localixa *= temp[tempixa];
							}
							if (flag)
							{
								ixaCounter++;
								printf("ixa %d\n", localixa * size);
								break;
							}
							printf("ixa %d\n dec 0\n", localixa * size);
							endedStructIndexes = 1;
							if (specialStructIND)
							{
								printf("ind\n");
								usedspecialStructIND = 1;
							}

							ixaCounter++;
							break;

							//printf("gets hereerrererereeerer\n");
							// Variable itr = mySymbolTable->Head;
							// int tempixa = ixaCounter;
							// while (itr)
							// {
							// 	if (!strcmp(itr->Name, leaf->data.sval->str))
							// 		break;
							// 	itr = itr->Next;
							// }
							// printf("got the symbol itr->name %s",itr)

							break;
						}

						itr = itr->Next;
					}
				}

				//printf("arrayRefrencing OUTT\nwith the leaf value of %s\n", leaf->data.sval->str);

				break;
			}

			if (tmpType == myArray)
			{
				//printf("entered mpType == myArray\nwith the leaf value of %s\n", leaf->data.sval->str);
				lastUsedArray = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
				strcpy(lastUsedArray, leaf->data.sval->str);

				//arrayRefrencing=1;
				break;
			}
			if (tezaherRefrence && nowRefrencing)
			{
				justDoneRefrencing = 1;
				//printf("entered if(tezaherRefrence && nowRefrencing)\nwith the leaf value of %s\n", leaf->data.sval->str);

				//printf("the last refrenced address is %d\nlastrefrenced struct", lastRefrencedAddress, lastRefrencedStruct);
				// Variable itr = mySymbolTable->Head;
				// for(; itr != NULL;itr=itr->Next){//printf("gets here\n");
				// 	if(isALabel(leaf->data.sval->str) && strcmp(leaf->data.sval->str,itr->label)){
				// 			printf("ldc %d\n",itr->Address);
				// 			nowRefrencing=0;
				// 			break;
				// 	}
				// }
				Variable var = getByAdress(lastRefrencedAddress);
				//				printf("now we got the var  %d\n",var->Type);
				Variable var2 = getByVarName(leaf->data.sval->str);
				//printf("now we got the var2  %s\n",var2->varName);
				var->targetAddress = var2->Address;
				printf("ldc %d\nsto\n");

				tezaherRefrence = 0;
				nowRefrencing = 0;
				break;
			}

			if (nowSelecting)
			{

				tmpRefrence = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
				strcpy(tmpRefrence, leaf->data.sval->str);

				//printf("entered nowSelecting\nwith the leaf value of %s\n", leaf->data.sval->str);
				Variable var = isAVariable(leaf->data.sval->str);
				int localIncrementAmount = 0;

				if (var)
				{
				//	printf("A:\n");
					printf("ldc %d\n", var->Address);
					lastRefrencedAddress = var->Address;
				//	printf("last refrenced address %d\nit was a struct name \n", lastRefrencedAddress);
					//lastRefrencedStruct = var;
					lastRefrencedStruct = lastRefrencedAddress;
					goto endThis;
				}
				else
				{
					if (lastRefrencedAddress > 0)
					{
				//			printf("B:\n");
						Variable firast = getByAdress(lastRefrencedStruct);
				//			printf("here i got %s inc %d->>>>>>>>>>>>>>>>>>%d \n", firast->Name, firast->increaseAmount, isAstructComponent(firast->label, leaf->data.sval->str));
						int tempo = isAstructComponent(firast->label, leaf->data.sval->str);
						if (isAstructComponent(firast->label, leaf->data.sval->str))
						{

							printf("inc %d\n", tempo);
							goto endThis;
						}

						while (firast)
						{

							if (!strcmp(firast->Name, leaf->data.sval->str))
							{
				//					printf("lets try the type maybe it will work %d\n", firast->Type);
								printf("inc %d\n", firast->increaseAmount);
								if (firast->Type == 5)
								{
								//	printf("ind?????????????%s\n", firast->Name);
								}
								goto endThis;
							}

							//printf("Skip2222222ped: %s for ya\n", firast->Name);
							firast = firast->Next;
						}

						// 						//printf("enters the line\n");
						// 						for (int i = 0; i < structCounter; i++)
						// 						{
						// 							printf("enters the line\n");

						// 							Variable itr = locUsedStructs[i]->thisSymbolTable->Head;
						// 							while(!itr->varName && strcmp(itr->varName,leaf->data.sval->str))
						// {
						// 	itr=itr->Next;

						// }
						// 							for (Variable j = ->thisSymbolTable->Head; j; j = j->Next)
						// 							{
						// 								if (!j->varName)
						// 								{

						// 									localIncrementAmount = j->Size + localIncrementAmount;

						// 									printf("skipped %s\nlocal increment amount now is %d\n",j->Name,localIncrementAmount);
						// 									continue;
						// 								}

						// 								if (!strcmp(j->varName, leaf->data.sval->str))
						// 								{

						// 									printf("localicrement %d\n it is a componet of a struct \n", localIncrementAmount);
						// 									printf("inc %d %s\n", localIncrementAmount, j->Name);

						// 									//lastRefrencedAddress = localIncrementAmount + lastRefrencedAddress;
						// 									printf("%d\n +++++++++++++\n", j->Type);
						// 									if (j->Type == myPointer)
						// 									{
						// 										pointedAddress = j->targetAddress;
						// 										printf("The last pointed Struct was %d\n", lastRefrencedStruct);
						// 										Variable var = getByAdress(lastRefrencedStruct);
						// 										printf("here i got the struct firas firas firas firas firas %s \n",var->label);
						// 										Struct str= getStructByName(var->label);
						// 										printf("here str is after doing the function %s",str->Name);
						// 										Variable itr = str->thisSymbolTable->Head;
						// 										localIncrementAmount=0;
						// 										while(itr){

						// 											if(!strcmp(j->Name,itr->Name)){
						// 												printf("ldc %d\n",localIncrementAmount);
						// 												break;
						// 											}
						// 											localIncrementAmount= localIncrementAmount+itr->Size;

						// 											itr=itr->Next;
						// 										}

						// 									}

						//	printf("updated for the first %d\n",lastRefrencedAddress);

						goto endThis;
					}
				//	printf("C:\n");
					localIncrementAmount = 0;
//printf("last line issssssssss :\n");
					var = getByAdress(lastRefrencedAddress);
//						printf("------------------here is houldnt ahve a problem %s %d \n", var->Name,localIncrementAmount);
					while (var)
					{
						if (!var->Name)
						{
							localIncrementAmount = var->Size + localIncrementAmount;
							var = var->Next;
//							printf("here is some new fo the test \n");
							continue;
						}
						//printf("tests the  %s\n", var->Name);

						if (!strcmp(var->Name, leaf->data.sval->str))
						{
							printf("inc %d\n", localIncrementAmount);
							lastRefrencedAddress = localIncrementAmount + lastRefrencedAddress;
//							printf("of %s\n", var->Name);
							goto endThis;
						}

						var = var->Next;
					}
				}
				//printf("here s %s\n",leaf->data.sval->str);
				//printf("gets here ");
				// 	Variable itr = mySymbolTable->Head;
				// 	for(; itr != NULL;itr=itr->Next){//printf("gets here\n");
				// 		if(isALabel(leaf->data.sval->str)){
				// 			//printf("tmptype here is %s\n",tmpType);
				// 			if(!strcmp(leaf->data.sval->str ,itr->label )){
				// 				printf("ldc %d\n",itr->Address);
				// 				break;
				// 			}

				// 		}
				// 		else{

				// 			if(!strcmp(leaf->data.sval->str ,itr->Name )){
				// 				printf("its a thingy we looking at %s \n",itr->Name);
				// 				printf("inc %d\n",itr->increaseAmount);
				// 				break;
				// 			}

				// 	}
				// }
			endThis:
				break;
			}
			if (tmpType == myStruct && onlyALabel == YES)
			{

				//printf("if(tmpType==myStruct &&onlyALabel==YES )\nwith the leaf value of %s\n", leaf->data.sval->str);

				Variable itr = locUsedStructs[lastEnteredIndex]->thisSymbolTable->Head;

				//printf("onlyALabel: %s tezaherPointer %d itr name %s \n",leaf->data.sval->str,tezaherPointer,itr->Name);
				//printf("tempType %d\n",tmpType);
				if (tezaherPointer)
				{
					//	printf("tezaterPointer: last before adding is %d\n", lastBeforeAdding);
					// lastBeforeAdding=lastBeforeAdding->Next;
					// printf("changed lastBeforeAdding %s to %s  \n",lastBeforeAdding->label,leaf->data.sval->str);
					// strcpy(lastBeforeAdding->label,leaf->data.sval->str);

					//printf(" tmpType==myStruct &&onlyALabel==YES  the current sympoltable is \n");
					// 	for(Variable itr = mySymbolTable->Head;itr;itr=itr->Next){
					// 	printf("changed the \nName: %s		Address: %d		Label: %s\n", itr->Name, itr->Address, itr->label);

					// }
					if (!lastBeforeAdding)
					{

						mySymbolTable->Head->varName = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);

						strcpy(mySymbolTable->Head->varName, leaf->data.sval->str);
						//printf("change the var name here:");
						mySymbolTable->Head->Type = myPointer;
						for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
						{
							//printf("Name: %s		Address: %d		Label : %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
						}

						onlyALabel = NO;
						tezaherPointer = 0;
						break;
					}

					if (!lastBeforeAdding->Next)
					{

						lastBeforeAdding->varName = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);

						strcpy(lastBeforeAdding->varName, leaf->data.sval->str);
						//printf("change the var name here:");
						lastBeforeAdding->Type = myPointer;
						for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
						{
							//	printf("Ngme: %s		Address: %d		Label : %d varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
						}

						onlyALabel = NO;
						tezaherPointer = 0;
						break;
					}
					//("ayahaaaaaaaaaaaaaaaaaaa %s\n", lastBeforeAdding->Next->Name);

					lastBeforeAdding->Next->varName = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
					//printf("ayahaaaaaaaaaaaaaaaaaaa %s\n", lastBeforeAdding->Next->Name);
					strcpy(lastBeforeAdding->Next->varName, leaf->data.sval->str);
					lastBeforeAdding->Next->Type = myPointer;
					//printf("change the var name here:");
					for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
					{
						//printf("Name: %s		Address: %d		Label : %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
					}
					onlyALabel = NO;
					tezaherPointer = 0;
				}
				else
				{
					//printf("not tezatasdsderPointer:\n");
					//	printf("not a pointer\n");
					if (!lastBeforeAdding)
					{

						mySymbolTable->Head->varName = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);

						strcpy(mySymbolTable->Head->varName, leaf->data.sval->str);
						//	printf("change the var name here:");
						for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
						{
							//	printf("Name: %s		Address: %d		Label : %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
						}

						onlyALabel = NO;
						tezaherPointer = 0;
						break;
					}

					//	printf("ayahaaaaaaaaaaaaaaaaaaa %s	\n", lastBeforeAdding->Name);
					if (!lastBeforeAdding->Next)
					{

						lastBeforeAdding->varName = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
						//printf("ayahaaaaaaaaaaaaaaaaaaa %s\n", lastBeforeAdding->Name);
						strcpy(lastBeforeAdding->varName, leaf->data.sval->str);
						//printf("change the var name here:");
						for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
						{
							//printf("Name: %s		Address: %d		Label : %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);

						//	printf("Name: %s		Address: %d		varname: %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
						}

						onlyALabel = NO;
						tezaherPointer = 0;
						break;
					}
					lastBeforeAdding->Next->varName = malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
					//printf("ayahaaaaaaaaaaaaaaaaaaa %s\n", lastBeforeAdding->Next->Name);
					strcpy(lastBeforeAdding->Next->varName, leaf->data.sval->str);
					//printf("change the var name here:");
					for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
					{
						//printf("Naaame: %s		Address: %d		Label : %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);

						//printf("Name: %s		Address: %d		varname: %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
					}
					// 				Variable itr = mySymbolTable->Head;
					// 				for(; itr != NULL;itr=itr->Next){//printf("gets here\n");
					// 					if(itr->label){
					// 						if(!strcmp(itr->label,locUsedStructs[itr->indexAtLoacArray]->Name)){
					// 							itr->varName= malloc(sizeof(leaf->data.sval->str) * sizeof(char) +1);
					// 							strcpy(itr->varName,leaf->data.sval->str);

					// //	printf("tmpType==myStruct &&onlyALabel==YES  else ************* the current sympoltable is \n");
					// 		for(Variable itr = mySymbolTable->Head;itr;itr=itr->Next){
					// 		printf("Name: %s		Address: %d		Label: %s varName %s\n", itr->Name, itr->Address, itr->label,itr->varName);

					// 	}

					// 						}
					// 					}

					// 				}
				}

				  // printf("here tmplabel %s\n",tmpLabel);

				onlyALabel = NO;
				tezaherPointer = 0;
				break;

				//	structIdentifiers[structIdentiferIndex]=malloc(sizeof(strlen(leaf->data.sval->str) * sizeof(char) + 1));
				//	strcpy(structIdentifiers[structIdentiferIndex++],leaf->data.sval->str);

				break;
			}
			if (nowRefrencing)
			{

				//printf("nowRefrencing: \nwith the leaf value of %s\n", leaf->data.sval->str);

				tmpName = (char *)malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
				strcpy(tmpName, leaf->data.sval->str);
				//printf("nowRefrencin  %s\n", tmpName);

				tmpRefrence = malloc(sizeof(char) * strlen(tmpName) + 1);
				strcpy(tmpRefrence, tmpName);
				tmpType = myStruct;
				
				//printf("saved the tmpName %s and tmpType %d\n", tmpName, tmpType);
				newGroom(NULL, myStruct, Scope);
				break;
			}

			if (tmpType == myStruct && nowRefrencing == NO)
			{

				locUsedStructs[structCounter] = malloc(sizeof(Struct));

				locUsedStructs[structCounter]->Name = (char *)malloc(sizeof(leaf->data.sval->str) + 1);

				if (!strcmp(leaf->data.sval->str, "main"))
				{
					//printf("main skipped\n");
					break;
				}
				strcpy(locUsedStructs[structCounter]->Name, leaf->data.sval->str);
				//printf("added the struct %s to locUsedStructs[structCounter] \n",leaf->data.sval->str);

				locUsedStructs[structCounter]->thisSymbolTable = (Symbol_table)malloc(sizeof(struct symbol_table));

				mySymbolTable = locUsedStructs[structCounter]->thisSymbolTable;

//				   	printf("the struct that we are using %s\n",locUsedStructs[structCounter]->Name);
				break;
			}
			if (isItNew)
			{
				tmpName = (char *)malloc(sizeof(char) * strlen(leaf->data.sval->str) + 1);
				strcpy(tmpName, leaf->data.sval->str);
				/*if (!isItThere(mySymbolTable, tmpName))*/ {
					addSymbol(mySymbolTable, newGroom(tmpName, tmpType, Scope));
//					printf("created the id %s and the type %d with address %d IN scope %d \n", tmpName, tmpType, base - 1, Scope);
				}
			}
			if (How == ADDRESS || How == BOTH)
			{
				if (Print == YES)
				{

					if (tezaherRefrence)
					{
						printf("ldc %d\n", isItThere(mySymbolTable, leaf->data.sval->str));
					}
					else
					{
						printf("ldc %d\n", isItThere(mySymbolTable, leaf->data.sval->str));
					}
					//tezaherPointer=0;
				}
			}
			if ((How == VALUE || How == BOTH) && Print == YES)
			{
				printf("ldc %d\n", isItThere(mySymbolTable, leaf->data.sval->str));
				//	printf("tezaherRefrence %d tezaherPointer %d\n", tezaherRefrence,tezaherPointer);
				if (printIND)
					printf("ind\n");
				//tezaherPointer=0;
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

			if (leaf->hdr.tok == FLOAT)
			{
				tmpType = myFLOAT;
				arrType = myFLOAT;
				//  printf("assign type myFLOAT\n");
			}
			else if (leaf->hdr.tok == INT) /////>>>>>>>>>>
			{
				tmpType = myINTEGER;
				arrType = myINTEGER;

				//printf("assign type myINTEGER\n");
			}
			else if (leaf->hdr.tok == DOUBLE)
			{

				tmpType = myDOUBLE;
				arrType = myDOUBLE;
				//printf("assign type myDOUBE\n");
				//printf("assign type myINTEGER\n");
			}
			else if (leaf->hdr.tok == myStruct)
			{
				tmpType = myStruct;
				//printf("assign type myStruct\n");
				arrType = myStruct;
			}
			else
			{
				isItNew = NO;
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

			if (arrayRefrencing)
			{
				arrayEnterdIndexes[arrayEnterdIndexesSize] = leaf->data.cval;
				printf("ldc %d\n", arrayEnterdIndexes[arrayEnterdIndexesSize]); //rember to retyrn ++ up
				calculateixa();
				arrayEnterdIndexesSize++;
				break;
			}

			if (tmpType == myArray)
			{
				int temp = leaf->data.cval;
				//printf("%d.........................\n",temp);
				arrayEnterdIndexes[arrayEnterdIndexesSize] = leaf->data.cval;
				//localSize*=

				//printf("enterd the dimesion %d\n", arrayEnterdIndexes[arrayEnterdIndexesSize]);

				//	localSize = (floor(localSize) * floor(leaf->data.dval));
				//printf("localsize %d\n", localSize);
				//itoa(leaf->data.dval, arrayEnterdIndexes, 10);
				//printf("%s\n", arrayEnterdIndexes);

				//printf("%d\n", arrayEnterdIndexesSize);
				// strcat(arrayEnterdIndexes, convertNumberIntoArray(leaf->data.dval));
				// strcat(arrayEnterdIndexes, "\n");
				// printf("should not ldc here %s \n", arrayEnterdIndexes);
				break;
			}
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
				printIND = 0;
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
			currentBreakIndex++;

			longStrTemp = (char *)malloc(sizeof("for_end") * sizeof(char) + 100);
			shorStrTemp = (char *)malloc(10);
			strcpy(longStrTemp, "for_end");
			//printf("%s",longStrTemp);
			itoa(forCounter, shorStrTemp, 10); // printf("Gets here \n");
			strcat(longStrTemp, shorStrTemp);
			//   printf("%s",longStrTemp);
			lastBreakLabel[currentBreakIndex] = (char *)malloc(sizeof(longStrTemp) * sizeof(char));
			strcpy(lastBreakLabel[currentBreakIndex], longStrTemp);

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

			casesCounter = 0;
			free(lastBreakLabel[currentBreakIndex]);
			currentBreakIndex--;

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
			//printf("hello bol o lo l o l o ");
			nowRefrencing = NO;
			tezaherPointer = 0;

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
				//printf("%d %d \n here we have a problem kind sir:\n",tezaherPointer,tezaherPointer);
				printIND = 1;
				specialStructIND = 1;
				wraper(root->rnode->rnode, Side, How, Print); //code_recur(root->lnode);
				Side = NONE;

				if (Print == YES)
					//	printf("%d %d \n here we have a problem kind sir:\n", ixaCounter, tezaherPointer);

					//	printf("****************************ixaCounter %d\n",ixaCounter);
					usedspecialStructIND = 0 && ixaCounter > 2 ? printf("ind\n") : 0;
				specialStructIND = 0;
				usedspecialStructIND = 0;
				printf("print\n");
				printIND = 0;

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
			//printf("before everything i guess \n");
			tmpType = myArray;
			//printf("before everything i guess %d \n", arrayEnterdIndexesSize);
			oneArrayCounter++;
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);

			arrayEnterdIndexesSize++;
			//printf("%d check array is here and abd abd abd adkjffhkdsfhklasdfj ilasd\n",checkarr);

			tmpType = myArray;
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			//printf("%d check array is here and abd abd abd adkjffhkdsfhklasdfj ilasd\n",checkarr);
			if (!checkarr)
			{
				//asdfasdf

				//printf("oneArrayCounter %d arrayEnteredIndexesSize %d\n",oneArrayCounter,arrayEnterdIndexesSize);
				if (oneArrayCounter == arrayEnterdIndexesSize + 1)
				{
					//		printf("here i should be able to declare one array i wish\n");
					//printf("%s\n", lastUsedArray);
					int *dimensions = malloc(sizeof(int) * 1000000);
					int size = 0;

					///////////////////////////
					for (int i = 1; i <= arrayEnterdIndexesSize; i++)
					{
						dimensions[i] = arrayEnterdIndexes[i];
						size++;
						//			printf("trying to multiply by %d\n", arrayEnterdIndexes[i]);
						localSize = localSize * arrayEnterdIndexes[i];
						//			printf("i %d: localsize: %d of size %d\n", i, localSize, arrayEnterdIndexesSize);
					}
					if (localSize == 1)
					{
						localSize = localSize * arrayEnterdIndexes[1];
						dimensions[1] = arrayEnterdIndexes[1];
					}
					//////////////////////////S
					//		printf("tempType here is %d, %d\n", arrType, localSize);
					for (int i = 1; i <= arrayEnterdIndexesSize; i++)
					{
						//printf("dimension %d : %d size is%d\n", i, dimensions[i], size);
					}
					for (int i = 1; i <= localSize; i++)
					{
						Variable Groom = (Variable)malloc(sizeof(struct variable));
						//check
						Groom->dimensionSize = size;
						Groom->arrayname = (char *)malloc(strlen(lastUsedArray) * sizeof(char) + 1);
						Groom->Next = NULL;
						Groom->Prev = NULL;
						//Groom->///add dimensions size and lxa almost done
						strcpy(Groom->arrayname, lastUsedArray);
						Groom->Scope = Scope;
						//printf(" the scope created333333333333 %d\n ", Scope);
						Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??
						Groom->Name = (char *)malloc(strlen(lastUsedArray) * sizeof(char) + 1);
						strcpy(Groom->Name, lastUsedArray);
						Groom->Address = base++;
						Groom->die = dimensions;
						Groom->dimensionSize = size;
						Groom->Type = arrType;
						//	Groom->indexAtLoacArray = getStructIndex(tmpName);

						addSymbol(mySymbolTable, Groom);
					}

					arrayEnterdIndexesSize = 0;
					oneArrayCounter = 0;
				}
			}
			//printf("after everything i guess %d arrType %d  \n", arrayEnterdIndexesSize, arrType);

			tmpType = NONE;
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
			//leaf->data.dval;
			//if(root->lnode->hdr)/////>>>>>>>>>>/////>>>>>>>>>>
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_COMP_DECL: //componets of the newly declared struct
						   /*struct component declaration - for HW2 */
						   //printf("struct node.");

			//STRUCTT

			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
												   //printf("helllllllllllllllllllllllllloooooooooooooooooo\n");
												   // printf("test the laef %d\n",root->rnode->hdr.tok);
			if (!root->rnode->hdr.tok)
			{

				tezaherPointer = 1;
			}

			//		onlyALabel=YES;

			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_DECL: //printf("enterd TN_DECL nowRefrencing %d\n",nowRefrencing);
			if (nowRefrencing)
			{

				//printf("now declaring %d\n", nowRefrencing);

				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			}
			else
			{

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
			}
			break;

		case TN_DECL_LIST:
			/*Maybe you will use it later */
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			break;

		case TN_DECLS:
			/*Maybe you will use it later */
			//	arrayEnterdIndexesSize=0;
			//	arrayEnterdIndexesSize = 0;
			checkarr = 1;
			//printf("yaravvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);

			if (arrayEnterdIndexesSize)
			{
				//	printf("************************************%s\n", lastUsedArray);
				int *dimensions = malloc(sizeof(int) * 1000000);
				int size = 0;

				///////////////////////////
				for (int i = 1; i <= arrayEnterdIndexesSize; i++)
				{
					dimensions[i] = arrayEnterdIndexes[i];
					size++;
					//		printf("trying to multiply by %d\n", arrayEnterdIndexes[i]);
					localSize = localSize * arrayEnterdIndexes[i];
					//		printf("i %d: localsize: %d of size %d\n", i, localSize, arrayEnterdIndexesSize);
				}
				if (localSize == 1)
				{
					localSize = localSize * arrayEnterdIndexes[1];
					dimensions[1] = arrayEnterdIndexes[1];
				}
				//////////////////////////S
				//	printf("tempType here is %d, %d\n", arrType, localSize);
				for (int i = 1; i <= arrayEnterdIndexesSize; i++)
				{
					//		printf("dimension %d : %d size %d\n", i, dimensions[i], size);
				}

				for (int i = 1; i <= localSize; i++)
				{
					Variable Groom = (Variable)malloc(sizeof(struct variable));
					//check
					Groom->dimensionSize = size;
					Groom->arrayname = (char *)malloc(sizeof(strlen(lastUsedArray) * sizeof(char)) + 1);
					Groom->Next = NULL;
					Groom->Prev = NULL;
					strcpy(Groom->arrayname, lastUsedArray);

					Groom->Name = (char *)malloc(sizeof(strlen(lastUsedArray) * sizeof(char)));
					strcpy(Groom->Name, lastUsedArray);
					Groom->Scope = Scope;
					Groom->dimensionSize = size;
					//printf(" the scope created %d\n ",Scope);
					Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??
					Groom->die = dimensions;
					Groom->Address = base++;
					Groom->Type = arrType;
					//	Groom->indexAtLoacArray = getStructIndex(tmpName);
Groom->arrayType=lastUsedArray;
					addSymbol(mySymbolTable, Groom);
				}

				arrayEnterdIndexesSize = 0;
			}
			//printf("malloced %d\n", localSize);
			localSize = 1;
			//printf("fefaefasfsdfasf sdfas\n");
			//arrayEnterdIndexesSize = 0;
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			if (arrayEnterdIndexesSize)
			{
				//printf("%s\n", lastUsedArray);
				int *dimensions = malloc(sizeof(int) * 1000000);
				int size = 0;

				///////////////////////////
				for (int i = 1; i <= arrayEnterdIndexesSize; i++)
				{
					dimensions[i] = arrayEnterdIndexes[i];
					size++;
					//		printf("trying to multiply by %d\n", arrayEnterdIndexes[i]);
					localSize = localSize * arrayEnterdIndexes[i];
					//		printf("i %d: localsize: %d of size %d\n", i, localSize, arrayEnterdIndexesSize);
				}
				if (localSize == 1)
				{
					localSize = localSize * arrayEnterdIndexes[1];
					dimensions[1] = arrayEnterdIndexes[1];
				}
				//////////////////////////S
				//	printf("tempType here is %d, %d\n", arrType, localSize);
				for (int i = 1; i <= arrayEnterdIndexesSize; i++)
				{
					//printf("dimension %d : %d size is%d\n", i, dimensions[i], size);
				}
				for (int i = 1; i <= localSize; i++)
				{
					Variable Groom = (Variable)malloc(sizeof(struct variable));
					//check
					Groom->dimensionSize = size;
					Groom->arrayname = (char *)malloc(sizeof(strlen(lastUsedArray) * sizeof(char)));
					Groom->Next = NULL;
					Groom->Prev = NULL;
					//Groom->///add dimensions size and lxa almost done
					strcpy(Groom->arrayname, lastUsedArray);
					Groom->Scope = Scope;
					//	printf(" the scope created 11111111111111111111111%d\n ", Scope);
					Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??
					Groom->Name = (char *)malloc(sizeof(strlen(lastUsedArray) * sizeof(char)));
					strcpy(Groom->Name, lastUsedArray);
					Groom->Address = base++;
					Groom->die = dimensions;
					Groom->dimensionSize = size;
					Groom->Type = arrType;
					Groom->arrayType=lastUsedArray;
					//	Groom->indexAtLoacArray = getStructIndex(tmpName);

					addSymbol(mySymbolTable, Groom);
				}

				arrayEnterdIndexesSize = 0;
			}
			//printf("malloced %d\n", localSize);
			localSize = 1;
			//printf("fefaefasfsdfasf sdfas\n");
			checkarr = 0;

			arrayEnterdIndexesSize = 0;
			//printf("yaravvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
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
			//printf("helllllllllllllllllllllllllloooooooooooooooooo\n");

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

			// here we declare the struct in the beggining
			// Struct/Union/Enum definition node
			//Struct/Union/Enum reference node
			//STRUCTT
			/*Maybe you will use it later */
			tmpType = myStruct;
			int tmpBase = base;
			base = 0;
			/*
			structTempName = (char*)malloc(sizeof(char)*sizeof(strlen(root->lnode->hdr.tok)) + 1);
			locUsedStructs[structCounter]=(Struct) malloc(sizeof(Struct));
			locUsedStructs[structCounter]->Name=(char*)malloc(sizeof(root->lnode->hdr.tok) + 1);
			strcpy(locUsedStructs[structCounter]->Name,root->lnode->hdr.tok);
			mySymbolTable=locUsedStructs[structCounter]->thisSymbolTable;
			*/
			//printf("declaring a struct\n");
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			locUsedStructs[structCounter]->thisSymbolTable = mySymbolTable;
			mySymbolTable = NULL;
			base = tmpBase;

			//printf("done declaring a struct !!!!!!!!!!!!!!!\n");
			Variable var = locUsedStructs[structCounter]->thisSymbolTable->Head;
			for (; var; var = var->Next)
			{
				//printf("Name: %s		Address: %d		Label : %s varName %s Type: %d\n", var->Name, var->Address, var->label, var->varName, var->Type);
			}

			//	Variable temp =locUsedStructs[structCounter-1]->thisSymbolTable->Head;

			//	printf("just finished dexclaring %s type is  %d\n",
			//		locUsedStructs[structCounter]->thisSymbolTable->Head->Name,
			//		locUsedStructs[structCounter]->thisSymbolTable->Head->Address);
			structCounter++;
			//temp=temp->Next;

			break;

		case TN_OBJ_REF:		 //REFRENCE TO A STRUCT
			nowRefrencing = YES; //printf("turned on\n");
								 //STRUCTT

			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			//printf("turned on\n");
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);

			//nowRefrencing=NO;	printf("turned off\n");
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
				printf("ujp %s\n", lastBreakLabel[currentBreakIndex]);
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
			//longStrTemp =(char * ) malloc(sizeof("switchEnd")*sizeof(char) + 15);
			//strcpy(longStrTemp,"switchEnd" );
			//itoa(shorStrTemp,switchCounter,15);
			// strcat(longStrTemp,shorStrTemp);
			//strcpy(lastJumpLabel[switchCounter++],longStrTemp);
			//lastJumpLabel[switchCounter++]=longStrTemp;

			/*
	crteate the break label here
*/
			switchCounter++;
			currentBreakIndex++;

			longStrTemp = (char *)malloc(sizeof("switchEnd") * sizeof(char) + 100);
			shorStrTemp = (char *)malloc(10);
			strcpy(longStrTemp, "switchEnd");
			//printf("%s",longStrTemp);
			itoa(switchCounter, shorStrTemp, 10); // printf("Gets here \n");
			strcat(longStrTemp, shorStrTemp);
			//   printf("%s",longStrTemp);
			lastBreakLabel[currentBreakIndex] = (char *)malloc(sizeof(longStrTemp) * sizeof(char));
			strcpy(lastBreakLabel[currentBreakIndex], longStrTemp);

			How = VALUE;
			printIND = 1;
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			printf("ixj swichEnd%d\n", switchCounter);
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);

			for (int i = casesCounter; i > 0; i--)
			{
				printf("ujp switch%d_case%d\n", switchCounter, i);
			}

			printf("swichEnd%d:\n", switchCounter);
			casesCounter = 0;
			free(lastBreakLabel[currentBreakIndex]);
			currentBreakIndex--;
			printIND = 0;
			break;

		case TN_INDEX:
			/*call for array - for HW2! */
			printIND = 1;

			arrayRefrencing = 1;
			Side = LEFT;
			//printf("enter left side:\n");

			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
			if (enteredBefore >= 1)
			{
				//printf("after left lastUsedArray %s\n here i should save a stuff father %s\n", lastUsedArray,father);
			}
			enteredBefore++;
			
			arrayRefrencing = 1;
			Side = RIGHT;
			//printf("finished the left side:\n");
			//printf("enterd right side:\n");
			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
					if (enteredBefore >1)
			{
				//printf("after right lastUsedArray %s\n return control to father %s\n", lastUsedArray,father);

				ixaCounter=tempixa;
				lastUsedArray=father;
				printf("ind\nixa 1\ndec 0\nind\n");
				


			}


			

// char* father;
// int  tempixa;

			arrayRefrencing = 0;
			enteredBefore--;
	
			//if(!enteredBefore){endedStructIndexes=0;}

			Side = NONE;
			printIND = 0;
			//printf("finished two sides:\n");
			break;

		case TN_DEREF:
			/*pointer derefrence - for HW2! */
			//printf("TN_DREF.................................................\n");
			wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);

			wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
			//printf(".................................................TN_DREF\n");
			//printf("tezaherPointer %d\n", tezaherPointer);
			printf("ind\n");
			break;

		case TN_SELECT:
			//printf("hello");
			/*struct case - for HW2! */
			if (root->hdr.tok == ARROW)
			{
				/*Struct select case "->" */
				/*e.g. struct_variable->x; */

					//printf("Enter Arrow here in this line can you see it now firas\n");
				nowSelecting = YES;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
				
				
					//printf("Exited arrow Arrow here in this line can you see it now firas\n");
			}
			else
			{
				/*Struct select case "." */
				/*e.g. struct_variable.x; */
					//printf("selector\n");
				nowSelecting = YES;

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
				if (ixaCounter > 1)
				{
					//		printf("****THIS IS OUR FIGHT THIS IS WHERER WE DIE \n local shit%d ixa counter %d lastUsedArray %s arrayRefrencing %d \n", tmpType, ixaCounter, lastUsedArray, arrayRefrencing);
					if (tezaherRefrence)
					{
						printf("sto\n");
					}
					else
					{
						if (printIND)
						{
							printf("ind\n");
						}
						//			printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^How %d\n", How);
						printf("sto\n");
					}

					tezaherRefrence = NO;
					justDoneRefrencing = 0;
					goto endThis;
				}

				if (Print == YES && !justDoneRefrencing)
					printf("sto\n");
				How = STAM;
				justDoneRefrencing = 0;
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
				printIND = 1;
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
				printIND = 0;
			}
			else if (root->hdr.tok == STAR_EQ)
			{
				/*Multiply equal assignment "*=" */
				/*e.g. x *= 5; */
				printIND = 1;
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
				printIND = 0;
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
			case B_AND:
				/* address token "&" */
				/* e.g. &x */
				//printf("LEFT --------->> %d\n", tezaherRefrence);
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				//printf(" --------->>LEFT %d\n", tezaherRefrence);
				How = ADDRESS;
				tezaherRefrence = YES;
				//	printf("RIGHT--------->> %d\n", tezaherRefrence);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	printf("--------->>RIGHT %d\n", tezaherRefrence);

				break;
			case CASE:
				/*you should not get here */
				//printf("caseNumber %d:\n",casesCounter);
				casesCounter++;
				printf("switch%d_case%d:\n", switchCounter, casesCounter);
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, NO);	   //code_recur(root->rnode);
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
				int temp = printIND;
				printIND = 1;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("add\n");
				How = STAM;
				printIND = 0;
				break;

			case MINUS:
				/*Minus token "-" */
				if (!root->lnode)
				{
					printIND = 1;
					How = VALUE;
					wraper(root->rnode, Side, How, Print); //	 code_recur(root->rnode);
					if (Print == YES)
						printf("neg\n");
					How = STAM;
					printIND = 0;
				}
				else
				{
					How = VALUE;
					////		Side = RIGHT;
					printIND = 1;
					wraper(root->lnode, Side, How, Print); //  code_recur(root->lnode);
					wraper(root->rnode, Side, How, Print); //  code_recur(root->rnode);
														   //		Side = LEFT;
					if (Print == YES)
						printf("sub\n");
					How = STAM;
					printIND = 0;
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
				printIND = 1;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				How = VALUE;
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("mul\n");
				//	How = ADDRESS;
				printIND = 0;
				break;

			case AND:
				/*And token "&&" */
				printIND = 1;
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("and\n");
				How = ADDRESS;
				printIND = 0;
				break;

			case OR:
				/*Or token "||" */
				printIND = 1;
				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //	Side = LEFT;
				if (Print == YES)
					printf("or\n");
				printIND = 0;
				break;

			case NOT:
				/*Not token "!" */
				printIND = 1;
				How = VALUE;
				//			Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //		Side = LEFT;
				if (Print == YES)
					printf("not\n");
				printIND = 0;
				break;

			case GRTR:
				/*Greater token ">" */
				printIND = 1;

				How = VALUE;
				//	Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //		Side = LEFT;
				if (Print == YES)
					printf("grt\n");

				printIND = 0;

				break;

			case LESS:
				/*Less token "<" */
				printIND = 1;
				How = VALUE;
				//	Side = RIGHT;
				printIND = 1;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
				//		Side = LEFT;
				printIND = 0;
				if (Print == YES)
					printf("les\n");

				printIND = 0;

				break;

			case EQUAL:
				/*Equal token "==" */
				How = VALUE;
				printIND = 1;
				//		Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //			Side = LEFT;
				if (Print == YES)
					printf("equ\n");
				printIND = 0;
				break;

			case NOT_EQ:
				/*Not equal token "!=" */
				printIND = 1;
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
				printIND = 0;

				break;

			case LESS_EQ:
				/*Less or equal token "<=" */
				printIND = 1;
				How = VALUE;
				//				Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //				Side = LEFT;
				if (Print == YES)
					printf("leq\n");
				printIND = 0;

				break;

			case GRTR_EQ:
				/*Greater or equal token ">=" */
				printIND = 1;
				How = VALUE;
				//				Side = RIGHT;
				wraper(root->lnode, Side, How, Print); //code_recur(root->lnode);
				wraper(root->rnode, Side, How, Print); //code_recur(root->rnode);
													   //						Side = LEFT;
				if (Print == YES)
					printf("geq\n");
				How = ADDRESS;
				printIND = 0;
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
			currentBreakIndex++;

			longStrTemp = (char *)malloc(sizeof("while_end") * sizeof(char) + 100);
			shorStrTemp = (char *)malloc(10);
			strcpy(longStrTemp, "while_end");
			//printf("%s",longStrTemp);
			itoa(whileCounter, shorStrTemp, 10); // printf("Gets here \n");
			strcat(longStrTemp, shorStrTemp);
			//   printf("%s",longStrTemp);
			lastBreakLabel[currentBreakIndex] = (char *)malloc(sizeof(longStrTemp) * sizeof(char) + 1);
			strcpy(lastBreakLabel[currentBreakIndex], longStrTemp);

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

			casesCounter = 0;
			free(lastBreakLabel[currentBreakIndex]);
			currentBreakIndex--;

			break;

		case TN_DOWHILE:

			Temp = doWhileCounter++;
			currentBreakIndex++;

			longStrTemp = (char *)malloc(sizeof("end_do") * sizeof(char) + 100);
			shorStrTemp = (char *)malloc(10);
			strcpy(longStrTemp, "end_do");
			//printf("%s",longStrTemp);
			itoa(doWhileCounter, shorStrTemp, 10); // printf("Gets here \n");
			strcat(longStrTemp, shorStrTemp);
			//   printf("%s",longStrTemp);
			lastBreakLabel[currentBreakIndex] = (char *)malloc(sizeof(longStrTemp) * sizeof(char) + 1);
			strcpy(lastBreakLabel[currentBreakIndex], longStrTemp);

			/*Do-While case */

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

			casesCounter = 0;
			free(lastBreakLabel[currentBreakIndex]);
			currentBreakIndex--;
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
	//printf("---------------------------------------\n");
	//printf("Showing the Symbol Table:\n");

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

Struct getStruct(char *Name)
{
	//printf("searching %s structCounter %d\n",Name);

	for (int i = 0; i < structCounter; i++)
	{
		//	printf("gets here \n");

		//	printf("searched %s\n",locUsedStructs[i]->Name);

		if (!strcmp(Name, locUsedStructs[i]->Name))
		{
			///	printf("found struct %s\n",locUsedStructs[i]->Name);
			return locUsedStructs[i];
		}
	}

	return NULL;
}

int getStructIndex(char *Name)
{
	//	printf("searching %s structCounter %d\n",Name);

	for (int i = 0; i < structCounter; i++)
	{
		//	printf("gets here \n");

		//	printf("searched %s\n",locUsedStructs[i]->Name);

		if (!strcmp(Name, locUsedStructs[i]->Name))
		{
			//printf("found struct %s\n",locUsedStructs[i]->Name);
			return i;
		}
	}

	return 0;
}

Variable newGroom(char *Name, TYPE Type, int Scope)
{
	if (nowRefrencing)
	{

			//printf("now refrencing a struct here i should add all of them\name %s\nbefore table\n:", Name);

		Struct tempStruct = getStruct(tmpName);
		if (!tempStruct)
		{
			//printf("do some magic in here so i dont fuck up everyting\nican use maybe %s", tmpName);
			lastBeforeAdding = mySymbolTable->Tail;
			if (!lastBeforeAdding)
			{
				lastBeforeAdding = mySymbolTable->Head;
			}

			Variable Groom = (Variable)malloc(sizeof(struct variable));
			if (!Groom)
				return NULL; //check
			Groom->Name = (char *)malloc(sizeof(strlen(tmpName) * sizeof(char)) + 1);
			Groom->Next = NULL;
			Groom->Prev = NULL;
			strcpy(Groom->Name, tmpName);
			Groom->Scope = Scope;
			//printf(" the scope created %d\n ",Scope);
			Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??

			Groom->increaseAmount += 0;

			Groom->Address = base++;
			Groom->Type = myPointer;
			//Groom->indexAtLoacArray = getStructIndex(tmpName);
			Groom->label = malloc(sizeof(char) * strlen(tmpName) + 1);
			strcpy(Groom->label, tmpName);

			addSymbol(mySymbolTable, Groom);
			//printf("just added %s , to the symbol table with label %s\n",Groom->Name,Groom->label);
			onlyALabel = YES;
			//	tmpLabel = malloc(sizeof(char) * strlen(tempStruct->Name) + 1);
			//	strcpy(tmpLabel, tempStruct->Name);

			lastBeforeAdding = Groom;

			//	printf("AFTER ADDING THE STRUCT pointer THE NEW SYMBOL TABLE IS: \n");
			for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
			{
				//	printf("Name: %s		Address: %d		Label : %s varName %s Type: %d\n", itr->Name, itr->Address, itr->label, itr->varName, itr->Type);
			}
			return NULL;
		}

		for (Variable itr = tempStruct->thisSymbolTable->Head; itr; itr = itr->Next)
		{
			//printf("Name: %s		Address: %d		Label : %s varName %s\n", itr->Name, itr->Address, itr->label, itr->varName);
		}
		Variable temp = tempStruct->thisSymbolTable->Head;
		int initialIncrease = 0;
		lastBeforeAdding = mySymbolTable->Tail;
		if (!lastBeforeAdding)
		{
			lastBeforeAdding = mySymbolTable->Head;
		}
		if (mySymbolTable->Tail)
				//printf("lastbefore adding %s++++++++\n", mySymbolTable->Tail->Name);
			while (temp)
			{
				if (temp->varName)
				{
					//printf("maybe the cure will be here if the name is null %s\n", temp->varName);
				}

				//printf("created a groom for %s\n", temp->varName);
				Variable Groom = (Variable)malloc(sizeof(struct variable));
				if (!Groom)
					return NULL; //check
				Groom->Name = (char *)malloc(sizeof(strlen(Name) * sizeof(char)));
				Groom->Next = NULL;
				Groom->Prev = NULL;
				strcpy(Groom->Name, temp->Name);
				Groom->Scope = Scope;
				//printf(" the scope created %d\n ",Scope);
				Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??

				Groom->increaseAmount += initialIncrease;
				initialIncrease += Groom->Size;
				Groom->Address = base++;
				//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%d", temp->Type);
				Groom->Type = temp->Type;
				Groom->indexAtLoacArray = getStructIndex(tmpName);
				Groom->label = malloc(sizeof(char) * strlen(tempStruct->Name) + 1);
				strcpy(Groom->label, tempStruct->Name);
				temp = temp->Next;
				addSymbol(mySymbolTable, Groom);
				//printf("just added %s , to the symbol table with label %s\n",Groom->Name,Groom->label);
				onlyALabel = YES;
				tmpLabel = malloc(sizeof(char) * strlen(tempStruct->Name) + 1);
				strcpy(tmpLabel, tempStruct->Name);
				//printf("helloweorld what the fuck is going ion");
				//printf("Type %d tmpType %d**********************************************\n", Type, tmpType);
			}
		//	printf("AFTER ADDING THE STRUCT THE NEW SYMBOL TABLE IS: \n");
		for (Variable itr = mySymbolTable->Head; itr; itr = itr->Next)
		{
			//printf("Name: %s		Type: %d		Label : %s varName %s Label : %d\n", itr->Name, itr->Type, itr->label, itr->varName, itr->Type);
		}

		return NULL;

		// lastEnteredIndex= getStructIndex(tmpName);
		// printf("creating groom for a struct %s \n", tempStruct->Name);
		// Variable temp = tempStruct->thisSymbolTable->Head;
		// int initialIncrease = 0;
		// lastBeforeAdding=mySymbolTable->Tail;
		// while (temp){

		// Variable Groom = (Variable)malloc(sizeof(struct variable));
		// if (!Groom)
		// 	return NULL; //check
		// Groom->Name = (char *)malloc(sizeof(strlen(Name) * sizeof(char)));
		// Groom->Next = NULL;
		// Groom->Prev = NULL;
		// strcpy(Groom->Name, temp->Name);
		// Groom->Scope = Scope;
		// //printf(" the scope created %d\n ",Scope);
		// Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??

		// Groom->increaseAmount+=initialIncrease;
		// initialIncrease+=Groom->Size;
		// Groom->Address = base++;
		// Groom->Type = Type;
		// Groom->indexAtLoacArray=getStructIndex(tmpName);
		// Groom->label = malloc(sizeof(char)*strlen(tempStruct->Name) +1 );
		// strcpy(Groom->label,tempStruct->Name);
		// temp=temp->Next;

		// addSymbol(mySymbolTable,Groom);
		// //printf("just added %s , to the symbol table with label %s\n",Groom->Name,Groom->label);
		// onlyALabel= YES;

		// tmpLabel=malloc(sizeof(char)*strlen(tempStruct->Name) +1);
		// strcpy(tmpLabel,tempStruct->Name);

		// }
		//printf("the new sympoltable is \n");
		//for(Variable itr = mySymbolTable->Head;itr;itr=itr->Next){
		//	printf("Name: %s		Address: %d		Label: %s\n", itr->Name, itr->Address, itr->label);

		//	}
	}
	else
	{
		//printf("creating groom for not struct\n");
		if (mySymbolTable->Tail)
			//printf("lastbefore adding %s**********\n", mySymbolTable->Tail->Name);
			lastBeforeAdding = mySymbolTable->Tail;
		if (!lastBeforeAdding)
		{
			lastBeforeAdding = mySymbolTable->Head;
		}
		//Struct tempStruct = getStruct(tmpName);

		//printf("trying to create a symbol, %s , %d\n", Name, Type);
		Variable Groom = (Variable)malloc(sizeof(struct variable));
		if (!Groom)
			return NULL; //check
		Groom->Name = (char *)malloc(sizeof(strlen(Name) * sizeof(char)) + 1);
		Groom->Next = NULL;
		Groom->Prev = NULL;
		strcpy(Groom->Name, Name);
		Groom->Scope = Scope;
		//printf(" the scope created %d\n ",Scope);
		Groom->Size = /*getSize(Type)*/ 1; ///do i really need it ??
		Groom->Address = base++;
		Groom->Type = Type;
		Groom->label = NULL;
		//	Groom->label = malloc(sizeof(char)*strlen() +1 );
		//	strcpy(Groom->label,tempStruct->Name);
		return Groom;
	}
	return NULL;
}

void addSymbol(Symbol_table Table, Variable Symbol)
{

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