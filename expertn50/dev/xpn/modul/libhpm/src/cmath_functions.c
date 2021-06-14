#include "cmath_functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef MSVC
#pragma warning(disable : 4022)
#endif

double sum4(double *var)
{
	int i;
	double res;
	res=0.0;
	for (i=0;i!=4;i++)
		{
		  res+=var[i];
		}
	return res;
}

void  set4(double *var,double value)
{
	//foreach4(var;{double *val},{val=value;});
	int i;
	for (i=0;i!=4;i++)
		{
			var[i] = value;
		}
}



double cpowi(double a, int b)
{
	return pow(a,b);
}

double cpulse(double t,double t_begin, double period,double width)
{
	double t2;
	t2 = t - period*(double)((int)(t/period));
	return ((t2 >= t_begin) && (t2 <= (t_begin+width)));
}

double cmax(double d1,double d2)
{
    if (d1 > d2) return d1; else return d2;
}
double cmin(double d1,double d2)
{
if (d1 < d2) return d1; else return d2;
}

double SUM_D(double *var[],int count)
{
	int i;
	double res;
	res=0.0;
	for (i=0;i!=count;i++)
		{
			res+=*var[i];
		}
	return res;
}

double *ARRAY_D_New(unsigned long count)
{
	unsigned char *ptr;
	unsigned long *_count;
	double *arrayd;
	ptr = (unsigned char*) malloc(sizeof(double)*count+sizeof(unsigned long));
	_count = (unsigned long*)(&ptr[0]);
	*_count = count;
	arrayd = (double*) (&ptr[sizeof(unsigned long)]);
	return arrayd; 
}

double *ARRAY_D_Resize(double *darray, unsigned long count)
{
	unsigned char *ptr;
	unsigned long *_count;
	if (darray == NULL)
		{
			return  ARRAY_D_New(count);
		}
	ptr = (unsigned char*)&(((unsigned char*) darray)[-(int)sizeof(unsigned long)]);
	ptr = (unsigned char*) realloc(ptr,sizeof(double)*count+sizeof(unsigned long));
	_count = (unsigned long*)(&ptr[0]);
	*_count = count;
	darray = (double*) (&ptr[sizeof(unsigned long)]);
	return darray;
}

unsigned long ARRAY_D_GetSize(double *var)
{
	return (unsigned long)(((unsigned char*) var)[-(int)sizeof(unsigned long)]);
}

void ARRAY_D_Clear(double *darray)
{
	unsigned long i;
	for (i=0;i!=ARRAY_D_GetSize(darray);i++)
		{
			darray[i] = 0.0;
		}
}

double *ARRAY_D_Delete(double *var)
{
	unsigned char *ptr;
	ptr = (unsigned char*)&(((unsigned char*) var)[-(int)sizeof(unsigned long)]);
	free(ptr);
	var = NULL;
	return NULL;
}


double **ARRAY_DP_New(unsigned long count)
{
	unsigned char *ptr;
	unsigned long *_count;
	double **arrayd;
	ptr = (unsigned char*) malloc(sizeof(double*)*count+sizeof(unsigned long));
	_count = (unsigned long*)(&ptr[0]);
	*_count = count;
	arrayd = (double**) (&ptr[sizeof(unsigned long)]);
	return arrayd;
}

double **ARRAY_DP_Resize(double **darray, unsigned long count)
{
	unsigned char *ptr;
	unsigned long *_count;
	if (darray == NULL)
		{
			return  ARRAY_DP_New(count);
		}
	ptr = (unsigned char*)&(((unsigned char*) darray)[-(int)sizeof(unsigned long)]);
	ptr = (unsigned char*) realloc(ptr,sizeof(double *)*count+sizeof(unsigned long));
	_count = (unsigned long*)(&ptr[0]);
	*_count = count;
	darray = (double**) (&ptr[sizeof(unsigned long)]);
	return darray;
}

double **ARRAY_DP_Delete(double **var)
{
	unsigned char *ptr;
	ptr = (unsigned char*)&(((unsigned char*) var)[-(int)sizeof(unsigned long)]);
	free(ptr);
	var = NULL;
	return NULL;
}

void ARRAY_DP_Clear(double **darray)
{
	unsigned long i;
	for (i=0;i!=ARRAY_DP_GetSize(darray);i++)
		{
			*darray[i] = 0.0;
		}
}

unsigned long ARRAY_DP_GetSize(double **var)
{
	return (unsigned long)(((unsigned char*) var)[-(int)sizeof(unsigned long)]);
}



C_Array* C_Array_Init(BOOLEAN ClearElement, UINT32 MemPerElement)
{
  C_Array* array;
  array = (C_Array*)malloc(sizeof(C_Array));
  array->list=NULL;
  array->Count=0;
  array->MemPerElement=MemPerElement;
  array->ClearElement=ClearElement;
  return array;
}

void C_Array_Done(C_Array* array)
{
	int i;
	if (array==NULL) return;
	if ((array->list== NULL ) || (array->Count==0)) {free(array);return;}
	if (array->ClearElement==TRUE)
		{
			for (i=0;i!=array->Count;i++)
			  {
				 free((void*)array->list[i]);
			  }
		}
	free(array->list);
	array->Count=0;
	array->list=NULL;
	free(array);
}

void C_Array_Add(C_Array* array,void **Element)
{
  array->Count++;
  array->list=(void**)realloc (array->list,array->MemPerElement* array->Count  );
  //memcpy(array->list[array->Count-1], Element, array->MemPerElement);
  array->list[array->Count-1]= Element;
}

C_DEBUG_VARS *C_DEBUG_VARS_Init()
{
	C_DEBUG_VARS *deb;
	deb = (C_DEBUG_VARS *) malloc(sizeof(C_DEBUG_VARS));
	deb->types = C_Array_Init(FALSE,sizeof(int));
	deb->pointers = C_Array_Init(FALSE,sizeof(double*));
	deb->names = C_Array_Init(TRUE,sizeof(char*));
  return deb;
}

void C_DEBUG_VARS_AddDouble(C_DEBUG_VARS *deb,const char* name, double *var)
{
   if (deb==NULL) return;
   C_Array_Add(deb->types,(int)0);
   C_Array_Add(deb->names,(void**)name);
   C_Array_Add(deb->pointers,(void**)var);
}

void C_DEBUG_VARS_AddFloat(C_DEBUG_VARS *deb,const char* name, float *var)
{
   if (deb==NULL) return;
   C_Array_Add(deb->types,(void **)((int)2));
   C_Array_Add(deb->names,(void**)name);
   C_Array_Add(deb->pointers,(void**)var);
}

void C_DEBUG_VARS_AddDoubleKnownWithLine(C_DEBUG_VARS *deb, double *var, int line)
{
   int i,count;
   count = deb->names->Count;
   for (i=0;i!=count;i++)
		{
	      double *d;
		  d=(double*)deb->pointers->list[i];
			if (d==var)
				{
				   char *S;
				   C_Array_Add(deb->types,(void**)(int)1);
				   S = (char*)malloc(strlen((char*)deb->names->list[i])+50);
				   sprintf(S,"%s\tLine: %d\t%f",(char*)deb->names->list[i],line,*var);
				   C_Array_Add(deb->names,(void**)S);
				   C_Array_Add(deb->pointers,(void**)NULL);
				}
		}

   
}


void C_DEBUG_VARS_Printf(C_DEBUG_VARS *deb)
{
   C_DEBUG_VARS_Print(deb,stdout);
}


void C_DEBUG_VARS_Print(C_DEBUG_VARS *deb,FILE *f)
{
	int i;
	for (i=0;i!=deb->names->Count;i++)
	  {
		  if ((int)deb->types->list[i]==0)
		  {
			double *d;
			d = (double*)deb->pointers->list[i];
		    fprintf(f,"%s\t%e\n",(char*)deb->names->list[i],*d);
		  } else
		  if ((int)deb->types->list[i]==1)
		  {
			  fprintf(f,"%s\n",(char*)deb->names->list[i]);
		  }
	  }
	
}

void C_DEBUG_VARS_Print_Values(C_DEBUG_VARS *deb,FILE *f)
{
	int i;
	for (i=0;i!=deb->names->Count;i++)
	  {
		  if ((int)deb->types->list[i]==0)
		  {
			double *d;
			d = (double*)deb->pointers->list[i];
		    fprintf(f,"%e",*d);
			if (i==deb->names->Count-1)
				{
					fprintf(f,"\n");
				} else
				{
					fprintf(f,",");
				}
		  }
		  if ((int)deb->types->list[i]==2)
		  {
			float *d;
			double d2;
			d = (float*)deb->pointers->list[i];
			d2 = (double) *d;
		    fprintf(f,"%e",d2);
			if (i==deb->names->Count-1)
				{
					fprintf(f,"\n");
				} else
				{
					fprintf(f,",");
				}
		  } 
	  }
	fflush(f);
}

void C_DEBUG_VARS_Print_Header(C_DEBUG_VARS *deb,FILE *f)
{
	int i;
	for (i=0;i!=deb->names->Count;i++)
	  {
		  if ((int)deb->types->list[i]==0)
		  {
		    fprintf(f,"%s",(char*)deb->names->list[i]);
			if (i==deb->names->Count-1)
				{
					fprintf(f,"\n");
				} else
				{
					fprintf(f,",");
				}
		  }
		  if ((int)deb->types->list[i]==2)
		  {
		    fprintf(f,"%s",(char*)deb->names->list[i]);
			if (i==deb->names->Count-1)
				{
					fprintf(f,"\n");
				} else
				{
					fprintf(f,",");
				}
		  } 
	  }
	fflush(f);
}

void C_DEBUG_VARS_Print_Index(C_DEBUG_VARS *deb,const char* filename)
{
	int i;
	FILE *f;
	f = fopen(filename,"w");
	for (i=0;i!=deb->names->Count;i++)
	  {
		  if (((int)deb->types->list[i]==0) || ((int)deb->types->list[i]==2))
		  {
		    fprintf(f,"%d\t%s",i,(char*)deb->names->list[i]);
			fprintf(f,"\n");
		  } 
	  }
	fclose(f);
}

double *C_DEBUG_VARS_SearchVarDouble(C_DEBUG_VARS *deb,const char* name) // returns pointer if var exists, or NULL
{
	double *var;
	int i;
	var = NULL;
	for (i=0;i!=deb->names->Count;i++)
		{
			if ((int)deb->types->list[i]==0)
			{
			   	if (strcmp((char*)deb->names->list[i],name)==0)
					{
						var = (double*)deb->pointers->list[i];
						break;
					}
			}
		}
	return var;
	
}

void C_DEBUG_VARS_Clear_Values(C_DEBUG_VARS *deb)
{
	double *var;
	float *var2;
	int i;
	var = NULL;
	var2 = NULL;
	for (i=0;i!=deb->names->Count;i++)
		{
			if ((int)deb->types->list[i]==0)
			{
				var = (double*)deb->pointers->list[i];
				*var = 0.0;
			}
			if ((int)deb->types->list[i]==2)
			{
				var2 = (float*)deb->pointers->list[i];
				*var2 = (float)0.0;
			}
		}
}

int C_DEBUG_VARS_Read_INI_File(C_DEBUG_VARS *deb,const char * filename)
{
	FILE *f;
	char *S,*S2,*S3,*Section;
	double *var;
	S=NULL;
	Section=NULL;
	var = NULL;
	f = fopen(filename,"r");
	if (f==NULL) return 1;
	while (1)
	 {
		   
		   S=cfgets_mem(f); // read one line
		   if ((feof(f) != 0) && (S==NULL))
                             { 
                               break;
                             } 
          
           if (S==NULL)      {
                               continue;
                             }
		   DeleteSpaceBegEnd(S);
		   if ((S[0]==';') || (S[0]=='#') || (strcmp(S,"")==0))   {
								if (S!=NULL) free(S);
                                S=NULL;
                                continue;
							  }
		   if (S[0]=='[')
							{
								S2 = (char*)malloc(strlen(S)+1);
								if (GetStringColumn(S2,S,"]",0)==0)
									{
										if (Section!=NULL)
											{
												free(Section);
											}
										Section = (char*)malloc(strlen(S2+1)+1);
										strcpy(Section,S2+1);
									}
								free(S2);
								free(S);
								continue;
							}
		   
		   
		   S2 = (char*)malloc(strlen(S)+1);
		   if (GetStringColumn(S2,S,"=",0)==0) 
	                           {
								   DeleteSpaceBegEnd(S2);
								   
								   if (Section==NULL)
										{
										   var = C_DEBUG_VARS_SearchVarDouble(deb,S2);
										} else
										{
										   S3 = (char*)malloc(strlen(Section)+strlen(S2)+3);
										   sprintf(S3,"%s.%s",Section,S2);
										   var = C_DEBUG_VARS_SearchVarDouble(deb,S3);
										   free(S3);
										}
								   if (var!=NULL)
										{
									       if (GetStringColumn(S2,S,"=",1)==0) 
											{
												*var = atof(S2);
											}
										}
							   }
		  
		   free(S);
		   free(S2);
		   if (feof(f) != 0)
                             { 
                               break;
                             } 
							//  fprintf(f2,"%s\n",S);fflush(f2);
	 }
	if (Section!=NULL)
		{
			free(Section);
		}
	fclose(f);
	return 0;
}

void C_DEBUG_VARS_Done(C_DEBUG_VARS *deb)
{
	C_Array_Done(deb->names);
	C_Array_Done(deb->pointers);
	C_Array_Done(deb->types);
	free(deb);
}


int GetStringColumn(char *dest,char *source,char *seperator,int column)
{
  char *pos;
  char *oldpos;
  int i;
  pos=source-strlen(seperator);;
  oldpos=pos;
  
  for (i=0;i!=column+1;i++)
  	{
  		oldpos=pos+strlen(seperator);
		pos = strstr(oldpos, seperator);
  			if (pos==NULL) 
  			{
	  			if (column==i)
  					{
	  					strncpy(dest, oldpos,strlen(oldpos));
						dest[strlen(oldpos)]=0;
	  					return 0;
					} else
					{
		    			dest=NULL;
		    			return 1;
					}
			}
		
	 }
	 strncpy(dest,oldpos,(int)(pos-oldpos));
	 dest[(int)(pos-oldpos)]=0;
  return 0;
}

char *cfgets_mem(FILE *stream)
{
  char *S; //memBuffer
  char *S2;
  unsigned int lenS;//reserved Memory
  unsigned int stdlenS;//momemory allocation buffer size
  int ready;
  int i;
  char ch;
  
  stdlenS=128;
  lenS=stdlenS;
  S=(char*)malloc(lenS);
  //lenSCount=0;
  ready=0;
  i=0;
  ch=0;
  
  while (ready==0)
    {
	  ch=fgetc(stream);
	  if ((ch==EOF) && (i==0)) 
	                {
	                    free(S);
	                    S=NULL;
                            return NULL;
                        }
	  if (i>=(int)lenS-2) 
	                { 
                            lenS+=stdlenS;
                            S=(char*)realloc(S,lenS);
                            if (S==NULL) 
                                {
                                  return NULL;
                                }
	                }
      if (ch==13) 
		 {i--;} else
	  if (ch==10)
	  	 {ready=1;} else
	  if (ch==EOF)
	  	 {ready=1;} else
	     {S[i]=ch;}	 
	  i++;
	}
   S[i-1]=0;
   S2=(char*)malloc(strlen(S)+1);
   strcpy(S2,S);
   free(S);
  return S2;
}

void DeleteSpaceBegEnd (char *str)
{
  if ((str == NULL) || (strlen(str)==0)) return;
  while (str[0]==' ')
  	{
		strcpy(str,&(str[1]));
	}
  while (str[strlen(str)-1]==' ')
  	{
		str[strlen(str)-1]=0;
	}
}

double get_infinity()
{
	double d1,d2;
	d1=1.0;
	d2=0.0;
	return d1 / d2;
}

static int ERROR_NO=0;
static int ERROR_LINE=0;
void _SetError(int No, int Line)
{
	if (ERROR_NO==0)
		{
			ERROR_NO=No;
			ERROR_LINE=Line;
		}
}

int  GetErrorNo()
{
	return ERROR_NO;
}
int  GetErrorLine()
{
	return ERROR_LINE;
}

char *GetErrorString()
{
	char *S;
	S=NULL;
	if (ERROR_NO==0)
		return S;
	S=(char*)malloc(128);
	sprintf(S,"Error %i at Line: %i",ERROR_NO,ERROR_LINE);
	return S;
}




C_EVENT * cEventReadFile(const char *filename, C_DEBUG_VARS *deb)
{
	FILE *f;
	char *S,*S2,*S3;
	double value_date,value_var;
	char *value_varname;
	double *deb_varpointer;
	C_EVENT * event;
	struct tm ti;
	S=NULL;
	event = NULL;
	f = fopen(filename,"r");
	if (f==NULL) return event;
	event = (C_EVENT*)malloc(sizeof(C_EVENT));
	event->time = NULL;
	event->var = NULL;
	event->value=NULL;
	event->size=0;
	event->old_simtime=-1.0;
	
	while (1)
	 {
		   if (S!=NULL) 
				{ 
					free(S);
					S=NULL;
				}
		   S=cfgets_mem(f); // read one line
		   if ((feof(f) != 0) && (S==NULL))
                             { 
                               break;
                             }
		   DeleteSpaceBegEnd (S);
		   if (strlen(S)==0) continue;
		   if (S[0]=='#') continue;
		   
		   S2=(char*)malloc(strlen(S)+1);
		   // Date:
		     
		   if (GetStringColumn(S2,S,"\t",0)==0)
			{
				
				if (strlen(S2) != 10)
					{
						free(S2);
						continue;
					}
					
				   S3 = (char*)malloc(strlen(S2)+1);
				   ti.tm_sec = 0;
				   ti.tm_min = 0;
				   ti.tm_hour = 0;
				   ti.tm_wday = 0;
				   ti.tm_isdst = 0;
				   ti.tm_yday = 0;
				  
				   if (GetStringColumn(S3,S2,".",0)==0)
						{
						   ti.tm_mday = atoi(S3);
						}
				   if (GetStringColumn(S3,S2,".",1)==0)
						{
						   ti.tm_mon = atoi(S3)-1;
						}
				   if (GetStringColumn(S3,S2,".",2)==0)
						{
						   ti.tm_year = atoi(S3)-1900;
						}
				   free(S3);S3=NULL;
				  
				   value_date = (double)mktime(&ti);
				     
			}
			 // Var name:
		   if (GetStringColumn(S2,S,"\t",1)==0)
			{
				    DeleteSpaceBegEnd (S2);
					if (strlen(S2)==0)
						{
							free(S2);
							S2=NULL;
							continue;
						}
					value_varname = (char*) malloc(strlen(S2)+1);
					strcpy(value_varname,S2);
			}
			// Var Value
			if (GetStringColumn(S2,S,"\t",2)==0)
			{
				    DeleteSpaceBegEnd (S2);
					if (strlen(S2)==0)
						{
							free(S2);
							S2=NULL;
							continue;
						}
				value_var = atof(S2);
			}
			free(S2);S2=NULL;
		deb_varpointer = C_DEBUG_VARS_SearchVarDouble(deb,value_varname);	
		if (deb_varpointer!=NULL)
			{
				event->size++;
				event->time = ARRAY_D_Resize(event->time,event->size);
				event->var = ARRAY_DP_Resize(event->var,event->size);
				event->value = ARRAY_D_Resize(event->value,event->size);
				event->time[event->size-1] = value_date;
				event->var[event->size-1] = deb_varpointer;
				event->value[event->size-1] = value_var;
			}
	 }
	 if (S!=NULL) free(S);
   return event;       
}

void cEventCheckValues(C_EVENT *event,double sim_time)
{
	unsigned long i;
	for (i=0;i!=event->size;i++)
		{
			if ((event->time[i] > event->old_simtime) && (event->time[i] <= sim_time))
				{
					*(event->var[i]) = event->value[i];
				}
		}
	event->old_simtime = sim_time;
}

