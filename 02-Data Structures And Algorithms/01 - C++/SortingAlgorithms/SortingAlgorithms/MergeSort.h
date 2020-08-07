#pragma once
#include "SortBase.h"
class MergeSort : public SortBase
{
public:
	MergeSort(int);
	~MergeSort();

	void Sort();

private:
	int arrayNum;
	int* array;

	void setArrayNum(int);

	//MergeSortFunctions
	void DivideArrayInTwo(int,int);
	void MergeArrays(int,int,int);
};
