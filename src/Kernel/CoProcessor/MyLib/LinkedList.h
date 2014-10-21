#ifndef LINKED_LIST
#define LINKED_LIST
//#pragma once
#include "QP_Utility.h"
#include "stdlib.h"
#include "stdio.h"

#define NUM_ENTRY 64
struct Bucket{
	Record data[NUM_ENTRY];
	Bucket *next;
	void init()
	{
		next=NULL;
	}
};


struct LinkedList
{
//attributes
	Bucket* header;
	Bucket* newBucket;
	int curEntry;
	int numBuckets;
//methods,

	void init()
	{
		newBucket=header=(Bucket*)malloc(sizeof(Bucket));
		header->init();		
		curEntry=0;
		numBuckets=0;
	}

	void destroy()
	{
		Bucket *b=header;
		Bucket *bnext=header;
		while(bnext!=NULL)
		{
			bnext=b->next;
			free(b);
			b=bnext;
		}
	}

	int size()
	{
		return (numBuckets*NUM_ENTRY)+curEntry;
	}

	void fill(Record * r)
	{
		if(curEntry==NUM_ENTRY)
		{
			if(newBucket->next==NULL)
			{
				newBucket->next=(Bucket*)malloc(sizeof(Bucket));
				newBucket->next->init();
			}
			newBucket=newBucket->next;
			EVAL((newBucket->data),r);
			curEntry=1;
			numBuckets++;
		}
		else
		{	
			EVAL((newBucket->data+curEntry),r);
			curEntry++;
		}
	}

	void copyToArray(Record* Rout)
	{
		/*Bucket *sBucket=header;
		int i=0;
		int cur=0;
		while(sBucket->next!=NULL)
		{
			for(i=0;i<NUM_ENTRY;i++)
			{
				EVAL((Rout+cur),(sBucket->data+i));
				cur++;
			}
			sBucket=sBucket->next;
		}
		//the last bucket
		for(i=0;i<curEntry;i++)
		{
			EVAL((Rout+cur),(sBucket->data+i));
			cur++;
		}*/
		//using memory copy
		Bucket *sBucket=header;
		int i=0;
		int cur=0;
		int bucketSizeInBytes=NUM_ENTRY*sizeof(Record);
		while(sBucket->next!=NULL)
		{
			memcpy(Rout+cur,sBucket->data,bucketSizeInBytes);
			cur=cur+NUM_ENTRY;
			sBucket=sBucket->next;
		}
		//the last bucket
		memcpy(Rout+cur,sBucket->data,curEntry*sizeof(Record));
		//cur=cur+curEntry;
	}
	int print()
	{
		Bucket *sBucket=header;
		int i=0;
		int cur=0;
		while(sBucket->next!=NULL)
		{
			for(i=0;i<NUM_ENTRY;i++)
			{
				////printf("%d; ",sBucket->data[i].value);
				cur++;
			}
			sBucket=sBucket->next;
		}
		//the last bucket
		for(i=0;i<curEntry;i++)
		{
			////printf("%d, ",(sBucket->data+i)->value);
			cur++;
		}
		return cur;
	}

	static void test()
	{		
		LinkedList* ll=(LinkedList*)malloc(sizeof(LinkedList));
		ll->init();		
		Record *r=(Record *)malloc(sizeof(Record));
		int i=0;		
		for(i=0;i<65;i++)
		{			
			r->value=i;
			ll->fill(r);
		}
	
		ll->print();
		__DEBUG__("test");
		ll->destroy();
	}
};

#endif

