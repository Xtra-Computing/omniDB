#include "stdio.h"
double _f(double array[] , int left , int right , int Max){
    if(left>right){
        return array[left+Max];
    }
 
    int middle=array[left];
    int _left=left;
    int _right=right;
    while(_left<_right){
        while((_left<_right)&&(array[_right]>=middle)){
            _right--;
        }
        array[_left]=array[_right];
 
        while((_left<_right)&&(array[_left]<=middle)){
            _left++;
        }
        array[_right]=array[_left];
    }
 
    if((_left-left)==Max){
		 return array[_left];
    }
 
    if((_left-left)<Max){
        return _f(array , _left+1 , right , Max-(_left-left)-1);
    }else{
	   return _f(array , left , _left-1 , Max);
    }
}
 
 
double f(double array[] , int length){
    return _f(array , 0 , length-1 , length/2);
}
 
void choise(double *a,int n)
{
	int i, j, k;
	double temp;
        for (i = 0; i < n - 1; i++) {
            k = i; 

            for (j = i + 1; j < n; j++) {
                if (a[k] > a[j]) {
                    k = j; 
                }
            }
            if (i != k) {
                temp = a[i];
                a[i] = a[k];
                a[k] = temp;
            }
        }
    }