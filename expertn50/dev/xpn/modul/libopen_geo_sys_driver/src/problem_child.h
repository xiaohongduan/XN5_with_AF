
#ifndef __PROBLEM_CHILD_H__
#define __PROBLEM_CHILD_H__

#include <problem.h>

class CRFProcess;

class problem_child:
	public Problem
{
public:
	problem_child(char* filename = NULL);
	~problem_child(void);
	int Euler_TimeDiscretize_TimeStep();
};
#endif
