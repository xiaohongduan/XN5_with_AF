#include "xylemwaterflow.h"

void swap(struct pff l[], int i, int j)
{  struct pff dummy;
   dummy=l[j];
   l[j]=l[i];
   l[i]=dummy;  
 }

int partions(struct pff l[],int low,int high)
{
    int prvotkey=l[low].i;
    while (low<high)
    {
        while (low<high && l[high].i>=prvotkey)
            --high;
        swap(l,high,low);
        while (low<high && l[low].i<=prvotkey) 
            ++low;
        swap(l,high,low);      
    }

    return low;
}

void qsort(struct pff l[],int low,int high)
{
    int prvotloc;
    if(low<high)
    {
        prvotloc=partions(l,low,high);    
        qsort(l,low,prvotloc); 
        qsort(l,prvotloc+1,high); 
    }
}

void quicksort(struct pff l[],int n)
{
    qsort(l,0,n); 
}
/*
void main()
{
    int a[11]={103,2,32,43,23,45,36,57,14,27,39};
    int b,c;
    for (b=0;b<=10;b++)
        printf("%3d ",a[b]);

    printf("\n");
    quicksort(a,10);

    for(c=0;c<=10;c++)
        printf("%3d ",a[c]);

}*/
