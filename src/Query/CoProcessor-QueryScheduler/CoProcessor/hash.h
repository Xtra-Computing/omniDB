//  Hash Table

#ifndef _hash_h
#define _hash_h
#include "db.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

struct HashTable {
    void init()
	{
		numTable=0;
		int i=0;
		for(i=0;i<MAX_TABLE_NUM;i++)
			tableName[i][0]='\0';
	}
    
//    void Dump(ostream&);
    bool RemoveEntry(char* key)
	{
		int i=0;
		for(i=0;i<MAX_TABLE_NUM;i++)
		{
			if(strcmp(tableName[i],key)==0)
			{
				break;
			}
		}
		if(i==MAX_TABLE_NUM)
		{
			return false;
		}
		else
		{
			tableName[i][0]='\0';
			return true;
		}
	}

	int AddEntry(char* key, int value)
	{
		strcpy(tableName[value],key);
		numTable++;
		return numTable;
	}

    bool Lookup(char* key, int* value)
	{
		int i=0;
		for(i=0;i<MAX_TABLE_NUM;i++)
		{
			if(strcmp(tableName[i],key)==0)
			{
				break;
			}
		}
		if(i==MAX_TABLE_NUM)
		{
			*value=-1;
			return false;
		}
		else
		{
			*value=i;
			return true;
		}
	}

	char tableName[MAX_TABLE_NUM][NAME_MAX_LENGTH];
	int numTable;
};

#endif /* _hash_h */
