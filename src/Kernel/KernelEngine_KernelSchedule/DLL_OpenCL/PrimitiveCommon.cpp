#include "PrimitiveCommon.h"

uint iSnapUp(uint dividend, uint divisor){
    return ((dividend % divisor) == 0) ? dividend : (dividend - dividend % divisor + divisor);
}

uint factorRadix2(uint& log2L, uint L){
    if(!L){
        log2L = 0;
        return 0;
    }else{
        for(log2L = 0; (L & 1) == 0; L >>= 1, log2L++);
        return L;
    }
}

int get2N( int rLen )
{
	unsigned int numRecordsR = 0;

	unsigned int size = rLen;
	unsigned int level = 0;
	while( size != 1 )
	{
		size = size/2;
		level++;
	}

	if( (1<<level) < rLen )
	{
		level++;
	}

	numRecordsR = (1<<level);
	return numRecordsR;
}



bool is2n(unsigned int i)
{
    if(i==0) return false;    
    else return (i&(i-1))==0;
}

unsigned int uintCeilingDiv(unsigned int dividend, unsigned int divisor)
{
	return (dividend + divisor - 1) / divisor;
}




unsigned int uintFloorLog(unsigned int base, unsigned int num)
{
	//unsigned int result = 0;

	//for(unsigned int temp = 0; temp <= num; temp *= base)
	//	result++;

	//return result;

	return int(logf(float(base))/logf(float(num)));
}



unsigned int uintCeilingLog(unsigned int base, unsigned int num)
{
	unsigned int result = 0;

	for(unsigned int temp = 1; temp < num; temp *= base)
		result++;

	return result;
}


unsigned int uintPower(unsigned int base, unsigned int pow)
{
	unsigned int result = 1;

	for(; pow; pow--)
		result *= base;

	return result;
}

