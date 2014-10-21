#ifndef PREDICATE_TREE_H
#define PREDICATE_TREE_H
#pragma once

#include "db.h"

#define PREDICATE_END -1
#define PREDICATE_UNKNOWN 0
#define PREDICATE_NUM 1
#define PREDICATE_COL 2
#define PREDICATE_AND 3
#define PREDICATE_OR 4
#define PREDICATE_EQUAL 5
#define PREDICATE_BIG 6
#define PREDICATE_SMALL 7
#define PREDICATE_NOTBIG 8
#define PREDICATE_NOTSMALL 9
#define PREDICATE_NOTEQUAL 10





struct PREDICATE_ARRAY_NODE
{
	int flag;
	int val;
	//int (*fp)(int a, int b);  
};

struct _PREDICATE_NODE
{
	_PREDICATE_NODE * left;
	_PREDICATE_NODE * right;
	char * opt;
};



class PredicateTree
{
public:
	_PREDICATE_NODE * root;
	char * CC_str;
	//int num_col;
	//char **columns;
	PREDICATE_ARRAY_NODE * array;
	PredicateTree(void);
	~PredicateTree(void);
	
	char * get_CC_predicate_string();
	int construct_CC_predicate_string(_PREDICATE_NODE* curNode,char * str);

	int construct_predicate_array(_PREDICATE_NODE* curNode,int index);
//	PREDICATE_NODE * construct_predicate_tree(char * str);
	_PREDICATE_NODE * construct_predicate_tree(char * str, int * index, int num_col, char **columns);
	void init();
	
};

COMP_TYPE getCompare(_PREDICATE_NODE * node, char **, char **);


#endif

