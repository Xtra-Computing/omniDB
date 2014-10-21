#include "db.h"
#include "QP_Utility.h"
#include "string.h"
int compareRecord (const void * a, const void * b)
{
  return ( ((Record*)a)->value - ((Record*)b)->value );
}


int compareRID (const void * a, const void * b)
{
  return ( ((Record*)a)->rid - ((Record*)b)->rid );
}


void linux_itoa(int value, char*  str, int radix)
{
    int  rem = 0;
    int  pos = 0;
 
    do
    {
        rem = value % radix ;
        value /= radix;
        str[pos++] = (char) ( rem + '0' );
    }while( value != 0 );
    str[pos] = '\0';
    //str = strrev(str);
    str=linux_strrev(str);
}

char* linux_strrev(char* szT)
{
    if ( !szT )                 // 处理传入的空�?
        return "";
    int i = (int)strlen(szT);
    int t = !(i%2)? 1 : 0;      // 检查串长度.
    for(int j = i-1 , k = 0 ; j > (i/2 -t) ; j-- )
    {
        char ch  = szT[j];
        szT[j]   = szT[k];
        szT[k++] = ch;
    }
    return szT;
}


int getOperandType(char * str)
{
	int i;
	bool hasLetter = false;

	if (str[0] == '#')
		return OPT_COL;

	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			hasLetter = true;
	}
	if (hasLetter)
		return OPT_OTHER;
	else
		return OPT_NUM;
}

