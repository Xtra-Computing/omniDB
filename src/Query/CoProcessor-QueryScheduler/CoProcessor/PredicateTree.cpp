#include "PredicateTree.h"
#include "string.h"
#include "db.h"
#include "stdlib.h"

PredicateTree::PredicateTree(void)
{
	
}

void PredicateTree::init()
{
	CC_str=get_CC_predicate_string();
	array = new PREDICATE_ARRAY_NODE[MAX_PREDICATE_NODE];
	int length = construct_predicate_array(root, 0);
	array[length].flag = PREDICATE_END;
}
PredicateTree::~PredicateTree(void)
{
}


char * PredicateTree::get_CC_predicate_string()
{
	int len;
	//char * str = new char [MAX_PREDICATE_STRING];
	char * str = (char*)malloc(sizeof(char)*MAX_PREDICATE_STRING);
	len = construct_CC_predicate_string(root, str);
	str[len] = '\0';
	return str;
}

int PredicateTree::construct_CC_predicate_string(_PREDICATE_NODE* curNode, char * str)
{
	int index = 0;

	if (curNode == NULL)
		return 0;

	////printf("con_str: %s\n", root->opt);
	
	if (curNode->left == NULL && curNode->right == NULL)
	{
		if (curNode->opt[0] == '#')
		{
			strcpy(str, "data[");
			strcpy(str + 5, curNode->opt + 1);
			int length = (int)strlen(str);
			str[length] = ']';
			return length + 1;
		}
		else
		{
			strcpy(str, curNode->opt);
			return (int)strlen(curNode->opt);
		}
	}
	else
	{
		str[index++] = '(';
		index += construct_CC_predicate_string(curNode->left, str + index);
		str[index++] = ')';
		if (strcmp(curNode->opt, "AND") == 0)
		{
			str[index++] = '&';
			str[index++] = '&';
		}
		else if (strcmp(curNode->opt, "OR") == 0)
		{
			str[index++] = '|';
			str[index++] = '|';
		}
		else if (strcmp(curNode->opt, ">=") == 0)
		{
			str[index++] = '>';
			str[index++] = '=';
		}
		else if (strcmp(curNode->opt, "<=") == 0)
		{
			str[index++] = '<';
			str[index++] = '=';
		}
		else if (strcmp(curNode->opt, "<>") == 0)
		{
			str[index++] = '!';
			str[index++] = '=';
		}
		else if (strcmp(curNode->opt, "=") == 0)
		{	
			str[index++] = '=';
			str[index++] = '=';
		}
		else if (strcmp(curNode->opt, ">") == 0)
			str[index++] = '>';
		else if (strcmp(root->opt, "<") == 0)
			str[index++] = '<';
		
		str[index++] = '(';
		index += construct_CC_predicate_string(curNode->right, str + index);
		str[index++] = ')';
		return index;
	}
}

int PredicateTree::construct_predicate_array(_PREDICATE_NODE* curNode,int index)
{
	if (curNode == NULL)
		return index;

	if (curNode->left)
		index = construct_predicate_array(curNode->left, index);
	if (curNode->right)
		index = construct_predicate_array(curNode->right, index);
	
	if (strcmp(curNode->opt, "AND") == 0)
		array[index].flag = PREDICATE_AND;
	else if (strcmp(curNode->opt, "OR") == 0)
		array[index].flag = PREDICATE_OR;
	else if (strcmp(curNode->opt, "=") == 0)
		array[index].flag = PREDICATE_EQUAL;
	else if (strcmp(curNode->opt, ">") == 0)
		array[index].flag = PREDICATE_BIG;
	else if (strcmp(curNode->opt, "<") == 0)
		array[index].flag = PREDICATE_SMALL;
	else if (strcmp(curNode->opt, ">=") == 0)
		array[index].flag = PREDICATE_NOTSMALL;
	else if (strcmp(curNode->opt, "<=") == 0)
		array[index].flag = PREDICATE_NOTBIG;
	else if (strcmp(curNode->opt, "<>") == 0)
		array[index].flag = PREDICATE_NOTEQUAL;
	else if (curNode->opt[0] == '#')
	{
		array[index].flag = PREDICATE_COL;
		array[index].val = atoi(curNode->opt + 1);
	}
	else
	{
		array[index].flag = PREDICATE_NUM;
		array[index].val = atoi(curNode->opt);
	}
	return index + 1;
}





COMP_TYPE getCompare(_PREDICATE_NODE * node, char ** col, char ** num)
{
	char * str = node->opt;
	int t1, t2;

	if (strcmp(str, ">") == 0 || strcmp(str, ">=") == 0 || strcmp(str, "=") == 0)
	{
		t1 = getOperandType(node->left->opt);
		t2 = getOperandType(node->right->opt);
		if (t1 == OPT_COL && t2 == OPT_NUM)
		{
			*col = node->left->opt;
			*num = node->right->opt;
			return CMP_BIGER;
		}
		if (t1 == OPT_NUM && t2 == OPT_COL)
		{
			*col = node->right->opt;
			*num = node->left->opt;
			return CMP_SMALLER;
		}
		return CMP_OTHER;
	}
	if (strcmp(str, "<") == 0 || strcmp(str, "<=") == 0)
	{
		t1 = getOperandType(node->left->opt);
		t2 = getOperandType(node->right->opt);
		if (t1 == OPT_COL && t2 == OPT_NUM)
		{
			*col = node->left->opt;
			*num = node->right->opt;
			return CMP_SMALLER;
		}
		if (t1 == OPT_NUM && t2 == OPT_COL)
		{
			*col = node->right->opt;
			*num = node->left->opt;
			return CMP_BIGER;
		}
		return CMP_OTHER;
	}
	return CMP_OTHER;
}




_PREDICATE_NODE * PredicateTree::construct_predicate_tree(char * str, int * index, int num_col, char **columns)
{
	int i, k, j = *index;
	_PREDICATE_NODE * node = new _PREDICATE_NODE;

	if (str[j] == '$')
	{
		*index = j + 2;
		return NULL;
	}


	for (i = j; str[i] != ','; i++);
	str[i] = '\0';
	node->opt = new char [i - j + 1];
	strcpy(node->opt, str + j);
	i++;

	//rename the column
	for (k = 0; k < num_col; k++)
		if (strcmp(node->opt, columns[k]) == 0)
		{
			node->opt[0] = '#';
			linux_itoa(k, node->opt + 1, 10);
			break;
		}

	//recursive
	if (strcmp(node->opt, "$") == 0)
	{
		delete node;
		node = NULL;
	}
	else
	{
		node->left = construct_predicate_tree(str, &i,num_col,columns);
		node->right = construct_predicate_tree(str, &i,num_col,columns);
	}
	*index = i;
	return node;
}
