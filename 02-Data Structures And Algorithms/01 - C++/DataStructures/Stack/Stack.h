#pragma once
#include "StackItem.h"

class StackItem;

class Stack
{
public :
	Stack(int val);

	void Push(int);
	int Pop();

private :
	StackItem* m_TopOfStack;
	int m_TotalCount;
};

