#include "gecros_tools.h"
#include <math.h>

//==========================================================================
// Function Prototypes
//==========================================================================
/*
int 		ReadSymbol(HANDLE,LPSTR);
int 		ReadDate(HANDLE,LPSTR);

int			ReadInt(HANDLE);
long		ReadLong(HANDLE);
double		Readdouble(HANDLE);
double		ReadDouble(HANDLE);
long double ReadLongDouble(HANDLE);

int DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
int LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber);
                                                              
int StringGetFromTo(LPSTR lpStr1,LPSTR lpStr2,int nBeg,int nEnd);                                                              
int doubleToString(double fNumber,int iDecimal,LPSTR lpString);
int doubleToWindow(HDC hdc,int X,int Y,double fNumber,int iDecimal);
int doubleWrite(HANDLE hFile,double fNumber,int iDecimal);

int EqualLenWrite(HANDLE hFile, double fValue, int Len);

double LIMIT(double v1, double v2, double x);

*/


//int doubleToString(double fNumber,int iDecimal,LPSTR lpString);
//double	ReadDouble(HANDLE);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds AFGENERATOR(double fInput, RESPONSE* pfResp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function interpolates linearly a value from a RESPONSE table using input fInput
//Inputs:	1. fInput	- the input value (-)
//			2. pfResp	- Pointer to the first element of the given RESPONSE table	
//Outputs:	The interpolated value at fInput (-)
//Functions Called: 
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double AFGENERATOR(double fInput, RESPONSE* pfResp)
	{
	RESPONSE *pResp, *pResp1;
	double	 fSlope,fOutput;
	int	  	 nPoints,i;
	
	nPoints = (int)(pfResp->fInput); 

	pResp = pfResp + 1;
  	if (fInput<=pResp->fInput)
		return pResp->fOutput;
        
	pResp = pfResp + nPoints;
	if (fInput>=pResp->fInput)
		return pResp->fOutput;

	pResp = pfResp + 1;   
	pResp1= pResp  + 1;
	for (i=1;i<nPoints;i++)
		{
		if (	((fInput>=pResp->fInput)&&(fInput<pResp1->fInput))
			||  ((fInput<=pResp->fInput)&&(fInput>pResp1->fInput))   )
			break;
		else
			{
			pResp ++;
			pResp1++;
			}
		}
                       
    fSlope = (pResp1->fOutput- pResp->fOutput)/(pResp1->fInput-pResp->fInput);                   
	fOutput= pResp->fOutput + fSlope*(fInput-pResp->fInput);

	return fOutput;
	}	


double FCNSW(double x, double v1, double v2, double v3)
	{                      
	double vReturn;
		                                               
	if (x<0)	vReturn=v1;
	if (x==0)	vReturn=v2;
	if (x>0)	vReturn=v3; 
	
	return vReturn;
	}

double LIMIT(double v1, double v2, double x)
	{                      
	double vReturn;
	
	if (v1>v2)	v1=v2;
	                                               
	if (x<=v1)			vReturn=v1;
	if (x>=v2)			vReturn=v2;
	if ((x>v1)&&(x<v2))	vReturn=x; 
	
	return vReturn;
	}

double NOTNUL(double x)
	{                      
	double vReturn;
	
	if (x==(double)0)	vReturn=(double)1;
	else                vReturn=x;                               
	
	return vReturn;
	}

double INSW(double x, double y1, double y2)
	{                      
	double vReturn;
	
	if (x<(double)0)	vReturn=y1;
	if (x>=(double)0)	vReturn=y2;
	
	return vReturn;
	}
	
double REAAND(double x, double y)
	{                      
	double vReturn;
	
	if ((x>(double)0)&&(y>(double)0))
	{
	 vReturn=(double)1;
	}
	else
	{
	 vReturn=(double)0;
	}
	
	return vReturn;
	}

double REANOR(double x, double y)
	{                      
	double vReturn;
	
	if ((x<=(double)0)&&(y<=(double)0))
	{
	 vReturn=(double)1;
	}
	else
	{
	 vReturn=(double)0;
	}
	
	return vReturn;
	}


/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double  abspowerDBL(double x, double y) {

	double sign,z;
	if (x < 0.0) {
		sign = -1.0;
		x  *= -1.0;
	}

	if (y < 0.0) {
		y *= -1.0;
		if (x != (double) 0.0)  x = 1.0/x;
	}
	z = (double)pow((double)x,(double)y);
	return z;
}

GDate *gecros_convert_str_to_gdate(char* S) {
	char yearstr[5],monthstr[3],daystr[3];
	int i;
	int day,month,year;
	GDate *test;

	if ((strlen(S) != 10)||(S==NULL))
		return ((GDate *)g_date_new_dmy((GDateMonth)01,(GDateMonth)01,(GDateMonth)9999));		

	for (i=0; i<4; i++) {
		yearstr[i] = S[i];
	}
	yearstr[4]=0;
	monthstr[0]=S[5];
	monthstr[1]=S[6];
	monthstr[2]=0;
	daystr[0]=S[8];
	daystr[1]=S[9];
	daystr[2]=0;

	day = atoi(daystr);
	month = atoi(monthstr);
	year = atoi(yearstr);
	test = (GDate*)g_malloc(sizeof(GDate));
	test = ((GDate *)g_date_new_dmy((GDateMonth)day,(GDateMonth)month,(GDateMonth)year));
	if (g_date_valid(test) == TRUE)
		return test;
	else
		return ((GDate *)g_date_new_dmy((GDateMonth)01,(GDateMonth)01,(GDateMonth)9999));
	
}







