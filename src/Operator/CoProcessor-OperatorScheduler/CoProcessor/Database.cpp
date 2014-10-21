#include "Database.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "QP_Utility.h"
#include "LinkedList.h"
#include "CoProcessor.h"
#include "CPU_Dll.h"
#include "Helper.h"
#include <iostream>
#include "Helper.h"
#include "OpenCL_DLL.h"
using namespace std;

extern int OP_rLen;
Database::Database(void)
{
#ifdef NON_LINUX_PLAT
	__DEBUG__("NON_LINUX_PLAT");
#else
	__DEBUG__("LINUX");
#endif
	numTable=0;
	numOriginalTable=0;
	nameIndex=(HashTable*)malloc(sizeof(HashTable));
	nameIndex->init();
	cc_indexObjs=(HashTable*)malloc(sizeof(HashTable)); 
	cc_indexObjs->init();
	tables=(Record**)malloc(sizeof(Record*)*MAX_TABLE_NUM);
	cpu_treeIndexes=(CUDA_CSSTree**)malloc(sizeof(CUDA_CSSTree*)*MAX_TABLE_NUM);
	gpu_treeIndexes=(CUDA_CSSTree**)malloc(sizeof(CUDA_CSSTree*)*MAX_TABLE_NUM);
	tPro=(TableProperty*)malloc(sizeof(TableProperty)*MAX_TABLE_NUM);
	int i=0;
	for(i=0; i<MAX_TABLE_NUM; i++)
	{
		tables[i]=NULL;
		cpu_treeIndexes[i]=NULL;
		gpu_treeIndexes[i]=NULL;
		tPro[i].OP_rLen=-1;
		tPro[i].sortField=0;
		tPro[i].cpu_treeindex=0;//there is no index
		tPro[i].gpu_treeindex=0;//there is no index
		numColumnInOTable[i]=0;
	}
}

Database::~Database(void)
{
	delete nameIndex;
	delete cc_indexObjs;
	int i=0;
	for(i=0; i<MAX_TABLE_NUM; i++)
	{
		if(tables[i]!=NULL)
			delete tables[i];
//		if(co_treeIndexes[i]!=NULL)
//			delete co_treeIndexes[i];
//		if(cpu_treeIndexes[i]!=NULL)
//			delete cpu_treeIndexes[i];
	}
	free(tables);
	free(tPro);
}

int Database::check(char *rName)
{
	int id;
	if(nameIndex->Lookup(rName,&id)==TRUE)
	{
		cout<<"table name duplicated: Database::AddTable(char *name):"<<rName<<endl;
		exit(-1);
	}
	if(numTable>=MAX_TABLE_NUM)
	{
		cout<<"too many tables, "<<rName<<endl;
		exit(-1);
	}
	int i=0;
	int resultID=0;
	for(i=0; i<MAX_TABLE_NUM; i++)
		if(tables[i]==NULL)
		{
			resultID=i;
			break;
		}
	return resultID;
}

/*int Database::createRandomTable(char* rName,cl_mem R, int OP_rLen)
{
	__DEBUG2__("createRandomTable",rName);
	int i=0;
	int tableID=check(rName);
	srand(numTable);
	for(i=0;i<OP_rLen;i++)
	{
		R[i].value=RAND(TEST_MAX);
		R[i].rid=i;
	}	
	//nameIndex->AddEntry(rName,numTable);
	//tables[tableID]=R;
	tPro[tableID].OP_rLen=OP_rLen;
	tPro[tableID].sortField=0;//not sorted.
	//numTable++;
	addTable(rName,R,OP_rLen);
	return (tableID);
}*/

/*int Database::createSortedTable(char* rName, Record *R, int OP_rLen)
{
	__DEBUG2__("createSortedTable",rName);
	int i=0;
	int tableID=check(rName);
	srand(numTable);
	for(i=0;i<OP_rLen;i++)
	{
		R[i].value=i+tableID;//RAND(TEST_MAX);
		R[i].rid=i;
	}	
	//qsort(R,OP_rLen,sizeof(Record),compareRecord);
	//nameIndex->AddEntry(rName,numTable);
	//tables[tableID]=R;
	tPro[tableID].OP_rLen=OP_rLen;
	tPro[tableID].sortField=1;//sorted.
	//numTable++;
	addTable(rName,R,OP_rLen);
	return (tableID);
}*/

int Database::dropTable(char* rName)
{
	int id=-1;
	if(nameIndex->Lookup(rName,&id)==TRUE)
	{
		delete tables[id];
		tables[id]=NULL;
		nameIndex->RemoveEntry(rName);
	}
	else
	{
		cout<<"table not found, "<<rName<<endl;
		exit(1);
	}
	return id;	
}

int Database::getTable(char* rName, cl_mem* Rout, int * OP_rLen)
{
	int id=-1;
	if(nameIndex->Lookup(rName,&id)==TRUE)
	{
			*OP_rLen=tPro[id].OP_rLen;
			int memSize=(*OP_rLen)*sizeof(Record);
			CL_CREATE(Rout, memSize);
			CopyCPUToGPU(*Rout,tables[id],memSize);//this step is correct
			//OP_bufferchecking(*Rout,memSize);
			//DATA_TO_GPU(memSize);
	}
	else
	{
		cout<<"table not found, "<<rName<<endl;
		exit(1);
	}
	return id;
}

int Database::test(void)
{
/*	int OP_rLen=10;
	Record *R=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *S=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record **Rout=(Record**)malloc(sizeof(Record*));
	Database db;
	db.createRandomTable("R1",R,OP_rLen);
	db.createSortedTable("R2",S,OP_rLen);
	db.dropTable("R1");
	db.dropTable("R2");*/
	return 0;
}

RET_VALUE Database::getTableProperty(int tableID, TableProperty* py)
{
	if(tables[tableID]!=NULL)
	{
		PY_EVAL(py,(this->tPro+tableID));
		return SUCCEED;
	}
	else
	return TABLE_NOT_FOUND;
}

int Database::getTableLength(char* tableName)
{
	int id=-1;
	int OP_rLen=-1;
	if(nameIndex->Lookup(tableName,&id)==TRUE)
	{
		OP_rLen=tPro[id].OP_rLen;
	}
	else
	{
		cout<<"table not found, "<<tableName<<endl;
		exit(1);
	}
	return OP_rLen;
}

/*int Database::createTreeIndex(char* tableName, char* columnName)
{
	__DEBUG2__("createCCTreeIndex",columnName);
	Record **Rin=(Record**) malloc(sizeof(Record*));
	int id=this->getTableID(columnName);
	int OP_rLen=this->tPro[id].OP_rLen;
	numIndex++;


	if(tPro[id].cpu_treeindex==0)
	{
		CL_BuildTreeIndex(tables[id], OP_rLen, &(cpu_treeIndexes[id]),0);
		tPro[id].cpu_treeindex=1;//indexed!!!
		//free(Rin);
	}

	if(tPro[id].gpu_treeindex==0)
	{
	
		CL_BuildTreeIndex(tables[id],OP_rLen,&(gpu_treeIndexes[id]),1);
		tPro[id].gpu_treeindex=1;//indexed!!!
	}

	return (id);
}*/

/*void Database::sortTable(char* tableName, char* columnName)
{
	__DEBUG2__("createCCTreeIndex",columnName);
	Record **Rin=(Record**) malloc(sizeof(Record*));
	int id=this->getTableID(columnName);
	int OP_rLen=tPro[id].OP_rLen;
	if(tPro[id].sortField==0)
	{
		Record* Rout=new Record[OP_rLen];
		CPU_Sort(tables[id],OP_rLen,Rout,OMP_SORT_NUM_THREAD);
		tPro[id].sortField=1;//indexed!!!
		memcpy(tables[id],Rout,sizeof(Record)*OP_rLen);
		delete Rout;
	}
}*/

void Database::removeTreeIndex(char* tableName, char* columnName)
{
	__DEBUG2__("removeTreeIndex",columnName);
	int id=this->getTableID(columnName);
	tPro[id].cpu_treeindex=0;//indexed!!!
	tPro[id].gpu_treeindex=0;//indexed!!!
	cpu_treeIndexes[id]=NULL;
	gpu_treeIndexes[id]=NULL;
	numIndex--;
}

int Database::loadDB(char* conFile,int Uplimit)
{
	FILE *src = fopen(conFile, "r");
	__DEBUG__(conFile);
	if(src!=NULL)
	{
		//load the data from the file.
		//printf("loading data from file, %s, ", conFile);
		char charBuf[NAME_MAX_LENGTH];
		char dFileName[NAME_MAX_LENGTH];
		int rid;
		int value;
		int len=0;
		while (!feof(src)) 
		{
			fgets(charBuf,NAME_MAX_LENGTH,src);
			len=(int)strlen(charBuf);
			if(len<=NAME_MAX_LENGTH) charBuf[len-1]='\0';
			if(strcmp(charBuf,"[Tables]")==0)
			{//tables
				while(!feof(src))
				{
					fgets(charBuf,NAME_MAX_LENGTH,src);//It is a table Name.
					len=(int)strlen(charBuf);
					if(len<=NAME_MAX_LENGTH) charBuf[len-1]='\0';
					__DEBUG__(charBuf);
					if(strcmp(charBuf,"[/Tables]")==0)
						break;
					fgets(dFileName,NAME_MAX_LENGTH,src);
					len=(int)strlen(dFileName);
					if(len<=NAME_MAX_LENGTH) dFileName[len-1]='\0';
#ifdef DB_FROM_FILE
					FILE *dbFile = fopen(dFileName, "r");
					if(dbFile!=NULL)
					{
						int OP_rLen=0;
						fscanf (dbFile, "%d", &OP_rLen);
						fscanf (dbFile, "%d", &OP_rLen);
						Record* R=(Record*)malloc(sizeof(Record)*OP_rLen);
						int numTuple=0;
						while(!feof(dbFile))
						{
							fscanf (dbFile, "%d", &rid);
							fscanf (dbFile, "%d", &value);
							R[numTuple].rid=rid;
							R[numTuple].value=value;
							numTuple++;
							if(numTuple==OP_rLen)
								break;
						}	
						fclose(dbFile);
					}
#else
						Record* R=(Record*)malloc(sizeof(Record)*OP_rLen);
						generateRand(R,Uplimit,OP_rLen,this->numTable);//make it adaptive...
#endif
						this->addTable(charBuf,R,OP_rLen);
						cout<<"importing "<<charBuf<<", size of, "<<OP_rLen<<endl;				
				}

			}
			else if(strcmp(charBuf,"[Indexes]")==0)
			{
			}

		}
		fclose(src);
	}
	else
	{
		__DEBUG__("ERROR: file not found");
	}
	return 0;
}

/*int Database::dumpDB(char* conFile, bool toWrite)
{
	if(toWrite)
	{
		int len=0;
		FILE *src = fopen(conFile, "r");
		if(src!=NULL)
		{
			//load the data from the file.
			printf("exporting data, configuration file: %s\n", conFile);
			char charBuf[NAME_MAX_LENGTH];
			char dFileName[NAME_MAX_LENGTH];
			while (!feof(src)) 
			{
				fgets(charBuf,NAME_MAX_LENGTH,src);
				len=(int)strlen(charBuf);
				if(len<=NAME_MAX_LENGTH) charBuf[len-1]='\0';
				if(strcmp(charBuf,"[Tables]")==0)
				{//tables
					while(!feof(src))
					{
						fgets(charBuf,NAME_MAX_LENGTH,src);//It is a table Name.
						len=(int)strlen(charBuf);
						if(len<=NAME_MAX_LENGTH) charBuf[len-1]='\0';
						if(strcmp(charBuf,"[/Tables]")==0)
							break;
						fgets(dFileName,NAME_MAX_LENGTH,src);	
						len=(int)strlen(dFileName);
						if(len<=NAME_MAX_LENGTH) dFileName[len-1]='\0';
						FILE *dbFile = fopen(dFileName, "w");
						if(dbFile!=NULL)
						{						
							Record** R=(Record **)malloc(sizeof(Record*));
							int OP_rLen=0;
							getTable(charBuf,R,&OP_rLen);
							fprintf(dbFile, "%d\n",OP_rLen);
							printf("%s, len, %d\n",dFileName,OP_rLen);
							for(int i=0;i<OP_rLen;i++)
							{
								fprintf(dbFile, "%d ",(*R)[i].rid);
								fprintf(dbFile, "%d\n",(*R)[i].value);
							}
							free(R);
						}
						fclose(dbFile);
						
					}

				}
				else if(strcmp(charBuf,"[Indexes]")==0)
				{
				}

			}
			fclose(src);
		}
		return 0;

	}
	else
	{
		printf("file not found: %s\n", conFile);
		return 0;
	}

}*/

int Database::addTable(char* rName, Record* data, int OP_rLen)
{
	int tableID=check(rName);
	nameIndex->AddEntry(rName,tableID);
	int id=0;
	bool rr=nameIndex->Lookup(rName,&id);
	//cout<<rName<<", "<<rr<<endl;
//	__DEBUG2__(rName,rr);
	tables[tableID]=data;
	tPro[tableID].OP_rLen=OP_rLen;
	//strcpy(allTableName[tableID],rName);
	//distinguish the original tableName and the columnName
	int strLen=(int)strlen(rName);
	char oTableName[NAME_MAX_LENGTH];
	char oColumnName[NAME_MAX_LENGTH];
	numTable++;
	for(int k=0; k<strLen; k++)
		if(rName[k]=='.')
		{
			strncpy(oTableName,rName,k);
			oTableName[k]='\0';
			strncpy(oColumnName,rName+k+1,strLen-k-1);
			oColumnName[strLen-k-1]='\0';
			break;
		}
	int i=0;
	for(i=0;i<numOriginalTable;i++)
	{
		if(strcmp(allTableName[i],oTableName)==0)
		{
			int len=(int)strlen(oColumnName);
			int oLen=(int)strlen(allColumnName[i]);
			strncpy(allColumnName[i]+oLen+1,oColumnName,len);
			allColumnName[i][oLen]=',';
			allColumnName[i][oLen+len+1]='\0';
			numColumnInOTable[i]++;
			break;
		}
	}
	if(i==numOriginalTable)//new original table
	{
		strcpy(allTableName[numOriginalTable],oTableName);
		allTableName[numOriginalTable][strlen(oTableName)]='\0';
		strcpy(allColumnName[numOriginalTable],oColumnName);
		allColumnName[numOriginalTable][strlen(oColumnName)]='\0';
		numColumnInOTable[i]=1;
		numOriginalTable++;
	}
	
	return (numTable-1);
}

//default size is 150K.
int Database::dbmbenchForMonet(int scale)
{
	int i=0;
	int t1,t2;
	int OP_rLen=DEFAULT_DB_SIZE;
	//T2.a1, primary key, for testing, we just let is be 1...DEFAULT_DB_SIZE, random.
	char dbFileName[120];
	sprintf(dbFileName,"./dbmbench/T2_%d.dat",OP_rLen);
	printf("dbmbenchForMonet, %s\n",dbFileName);
	FILE *dbFile = fopen(dbFileName, "w");
	Record *T2A1=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T2A2=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T2A3=(Record*)malloc(sizeof(Record)*OP_rLen);
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			T2A1[i].value=i;
		}
		//randomized the array.
		Record TMP;
		for(i=0;i<OP_rLen;i++)
		{
			t1=RAND(OP_rLen);
			t2=RAND(OP_rLen);
			SWAP((&T2A1[t1]),(&T2A1[t2]),TMP);
		}
		for(i=0;i<OP_rLen;i++)
		{
			T2A1[i].rid=i;
		}
		for(i=0;i<OP_rLen;i++)
		{
			T2A2[i].value=RAND(20000*scale);
		}
		for(i=0;i<OP_rLen;i++)
		{
			T2A3[i].value=RAND(50*scale);
		}
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(T2A1)[i].value);
			fprintf(dbFile, "%d ",(T2A2)[i].value);
			fprintf(dbFile, "%d\n",(T2A3)[i].value);
		}
	}
	fclose(dbFile);
	free(T2A1);
	free(T2A2);
	free(T2A3);
	
	//////////////////////////////////////////////////////////////////////////////
	///////////////T2////////////////////////////////////////////////////////
	OP_rLen=DEFAULT_DB_SIZE*scale;
	sprintf(dbFileName,"./dbmbench/T1_%d.dat",OP_rLen);
	printf("dbmbenchForMonet, %s\n",dbFileName);
	dbFile = fopen(dbFileName, "w");
	Record *T1A1=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T1A2=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T1A3=(Record*)malloc(sizeof(Record)*OP_rLen);
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			T1A1[i].value=RAND(DEFAULT_DB_SIZE);
		}
		for(i=0;i<OP_rLen;i++)
		{
			T1A2[i].value=RAND(20000*scale);
		}
		for(i=0;i<OP_rLen;i++)
		{
			T1A3[i].value=RAND(50*scale);
		}
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(T1A1)[i].value);
			fprintf(dbFile, "%d ",(T1A2)[i].value);
			fprintf(dbFile, "%d\n",(T1A3)[i].value);
		}
	}
	fclose(dbFile);
	free(T1A1);
	free(T1A2);
	free(T1A3);
	return 0;
}


//default size is 150K.
int Database::dbmbenchForMonetLoad(int scale)
{
	int i=0;
	int t1,t2;
	int OP_rLen=DEFAULT_DB_SIZE;
	//T2.a1, primary key, for testing, we just let is be 1...DEFAULT_DB_SIZE, random.
	char dbFileName[120];
	sprintf(dbFileName,"./dbmbench/T2_%d.dat",OP_rLen);
	printf("dbmbenchForEaseDB, %s\n",dbFileName);
	FILE *dbFile = fopen(dbFileName, "r");
	Record *T2A1=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T2A2=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T2A3=(Record*)malloc(sizeof(Record)*OP_rLen);
	if(dbFile!=NULL)
	{						
		for(int i=0;i<OP_rLen;i++)
		{
			fscanf(dbFile, "%d",&((T2A1)[i].value));
			fscanf(dbFile, "%d",&((T2A2)[i].value));
			fscanf(dbFile, "%d",&((T2A3)[i].value));
			if((T2A1)[i].value<0 || (T2A2)[i].value<0 || (T2A3)[i].value<0)
			{
				printf("error reading\n");
				break;
			}
			(T2A1)[i].rid=i;
			(T2A2)[i].rid=i;
			(T2A3)[i].rid=i;
		}
	}
	fclose(dbFile);
	int id;
	id=this->addTable("T2.a1", T2A1,OP_rLen);
	id=this->addTable("T2.a2", T2A2,OP_rLen);
	id=this->addTable("T2.a3", T2A3,OP_rLen);
	
	
	//////////////////////////////////////////////////////////////////////////////
	///////////////T2////////////////////////////////////////////////////////
	OP_rLen=DEFAULT_DB_SIZE*scale;
	sprintf(dbFileName,"./dbmbench/T1_%d.dat",OP_rLen);
	printf("dbmbenchForEaseDB, %s\n",dbFileName);
	dbFile = fopen(dbFileName, "r");
	Record *T1A1=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T1A2=(Record*)malloc(sizeof(Record)*OP_rLen);
	Record *T1A3=(Record*)malloc(sizeof(Record)*OP_rLen);
	if(dbFile!=NULL)
	{						
		for(int i=0;i<OP_rLen;i++)
		{
			fscanf(dbFile, "%d",&((T1A1)[i].value));
			fscanf(dbFile, "%d",&((T1A2)[i].value));
			fscanf(dbFile, "%d",&((T1A3)[i].value));
			if((T1A1)[i].value<0 || (T1A2)[i].value<0 || (T1A3)[i].value<0)
			{
				printf("error reading in 2\n");
				break;
			}
			(T1A1)[i].rid=i;
			(T1A2)[i].rid=i;
			(T1A3)[i].rid=i;
		}
	}
	fclose(dbFile);
	id=this->addTable("T1.a1", T1A1,OP_rLen);
	id=this->addTable("T1.a2", T1A2,OP_rLen);
	id=this->addTable("T1.a3", T1A3,OP_rLen);
	return 0;
}


int Database::dbmbench(int scale)
{
	int size=DEFAULT_DB_SIZE;
	int i=0;
	int t1,t2;
	int OP_rLen=DEFAULT_DB_SIZE;
	//T2.a1, primary key, for testing, we just let is be 1...DEFAULT_DB_SIZE, random.
	Record *R=(Record*)malloc(sizeof(Record)*OP_rLen);
	for(i=0;i<OP_rLen;i++)
	{
		R[i].value=i;
	}
	//randomized the array.
	Record TMP;
	for(i=0;i<OP_rLen;i++)
	{
		t1=RAND(OP_rLen);
		t2=RAND(OP_rLen);
		SWAP((&R[t1]),(&R[t2]),TMP);
	}
	for(i=0;i<OP_rLen;i++)
	{
		R[i].rid=i;
	}
	this->addTable("T2.a1",R,OP_rLen);
	//T2.a2.dat
	Record *R_T2A2=(Record*)malloc(sizeof(Record)*OP_rLen);
	for(i=0;i<OP_rLen;i++)
	{
		R_T2A2[i].value=RAND(20000);
		R_T2A2[i].rid=i;
	}
	this->addTable("T2.a2",R_T2A2,OP_rLen);
	//T2.a3.dat
	Record *R_T2A3=(Record*)malloc(sizeof(Record)*OP_rLen);
	for(i=0;i<OP_rLen;i++)
	{
		R_T2A3[i].value=RAND(50);
		R_T2A3[i].rid=i;
	}
	this->addTable("T2.a3",R_T2A3,OP_rLen);
//////////////////////////////////////////////////////////////////////////////
	///////////////T2////////////////////////////////////////////////////////
	OP_rLen=DEFAULT_DB_SIZE*scale;
	//T1.a1.dat
	Record* R_T1A1=(Record*)malloc(sizeof(Record)*OP_rLen);
	for(i=0;i<OP_rLen;i++)
	{
		R_T1A1[i].value=RAND(150000);
		R_T1A1[i].rid=i;
	}
	this->addTable("T1.a1",R_T1A1,OP_rLen);
	//T1.a2.dat
	Record* R_T1A2=(Record*)malloc(sizeof(Record)*OP_rLen);
	for(i=0;i<OP_rLen;i++)
	{
		R_T1A2[i].value=RAND(20000);
		R_T1A2[i].rid=i;
	}
	this->addTable("T1.a2",R_T1A1,OP_rLen);
	//T1.a3.dat
	Record* R_T1A3=(Record*)malloc(sizeof(Record)*OP_rLen);
	for(i=0;i<OP_rLen;i++)
	{
		R_T1A3[i].value=RAND(50);
		R_T1A3[i].rid=i;
	}
	this->addTable("T1.a3",R_T1A3,OP_rLen);
	cout<<"T1, "<<OP_rLen<<"; T2, "<<DEFAULT_DB_SIZE<<endl;
	cout<<"total database size, "<<sizeof(Record)*DEFAULT_DB_SIZE*3*(scale+1)/1024/1024<<" MB,"<<endl;
	return 0;
	/*int size=DEFAULT_DB_SIZE*scale;
	int i=0;
	int t1,t2;
	int OP_rLen=DEFAULT_DB_SIZE*scale;
	//T2.a1, primary key, for testing, we just let is be 1...DEFAULT_DB_SIZE, random.
	FILE *dbFile = fopen("./dbmbench/T2.a1.dat", "w");
	Record *R=(Record*)malloc(sizeof(Record)*OP_rLen);
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			R[i].value=i;
		}
		//randomized the array.
		Record TMP;
		for(i=0;i<OP_rLen;i++)
		{
			t1=RAND(OP_rLen);
			t2=RAND(OP_rLen);
			SWAP((&R[t1]),(&R[t2]),TMP);
		}
		for(i=0;i<OP_rLen;i++)
		{
			R[i].rid=i;
		}
		fprintf(dbFile, "%d\n",OP_rLen);
		fprintf(dbFile, "%d\n",OP_rLen);
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(R)[i].rid);
			fprintf(dbFile, "%d\n",(R)[i].value);
		}
	}
	fclose(dbFile);
	//T2.a2.dat
	dbFile = fopen("./dbmbench/T2.a2.dat", "w");
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			R[i].value=RAND(20000*scale);
			R[i].rid=i;
		}
		fprintf(dbFile, "%d\n",OP_rLen);
		fprintf(dbFile, "%d\n",OP_rLen);
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(R)[i].rid);
			fprintf(dbFile, "%d\n",(R)[i].value);
		}
	}
	fclose(dbFile);
	//T2.a3.dat
	dbFile = fopen("./dbmbench/T2.a3.dat", "w");
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			R[i].value=RAND(50*scale);
			R[i].rid=i;
		}
		fprintf(dbFile, "%d\n",OP_rLen);
		fprintf(dbFile, "%d\n",OP_rLen);
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(R)[i].rid);
			fprintf(dbFile, "%d\n",(R)[i].value);
		}
	}
	fclose(dbFile);
//////////////////////////////////////////////////////////////////////////////
	///////////////T2////////////////////////////////////////////////////////
	OP_rLen=DEFAULT_DB_SIZE*scale;
	free(R);
	R=(Record*)malloc(sizeof(Record)*OP_rLen);
	//T1.a1.dat
	dbFile = fopen("./dbmbench/T1.a1.dat", "w");
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			R[i].value=RAND(150000*scale);
			R[i].rid=i;
		}
		fprintf(dbFile, "%d\n",OP_rLen);
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(R)[i].rid);
			fprintf(dbFile, "%d\n",(R)[i].value);
		}
	}
	fclose(dbFile);
	//T1.a2.dat
	dbFile = fopen("./dbmbench/T1.a2.dat", "w");
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			R[i].value=RAND(20000*scale);
			R[i].rid=i;
		}
		fprintf(dbFile, "%d\n",OP_rLen);
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(R)[i].rid);
			fprintf(dbFile, "%d\n",(R)[i].value);
		}
	}
	fclose(dbFile);
	//T1.a3.dat
	dbFile = fopen("./dbmbench/T1.a3.dat", "w");
	if(dbFile!=NULL)
	{						
		for(i=0;i<OP_rLen;i++)
		{
			R[i].value=RAND(50*scale);
			R[i].rid=i;
		}
		fprintf(dbFile, "%d\n",OP_rLen);
		for(int i=0;i<OP_rLen;i++)
		{
			fprintf(dbFile, "%d ",(R)[i].rid);
			fprintf(dbFile, "%d\n",(R)[i].value);
		}
	}
	fclose(dbFile);
	free(R);
	return 0;*/
}
int Database::getTableID(char* tableName)
{
	int id=-1;
	if(nameIndex->Lookup(tableName,&id)==TRUE)
	{
		return id;
	}
	else
	{
		cout<<"table not found, "<<tableName<<endl;
		return -1;
	}
	return id;
}
