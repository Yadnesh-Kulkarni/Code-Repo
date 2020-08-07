#pragma once
#include "SortBase.h"
class BubbleSort : public SortBase
{
public:
	BubbleSort();
	BubbleSort(int);
	~BubbleSort();

	void Sort();

private:
	int* array;
	int arrayNum;

	void setArrayNum(int);
};

