#pragma once
#include "SortBase.h"
class SelectionSort : public SortBase
{
public :
	SelectionSort();
	SelectionSort(int);
	~SelectionSort();

	void Sort();
private :
	void setArrayNum(int);

	int arrayNum = 0;
	int* array = nullptr;
};

