#pragma once
class SortBase
{
protected:
	int* GenerateArray(int length);
	void DegenArray(int *array);
	void PrintArray(int *array,int length);

	virtual void Sort();
};

