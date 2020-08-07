#include "SelectionSort.h"
#include <iostream>

SelectionSort::SelectionSort()
{
	std::cout << "Invalid Constructor Called" << std::endl;
}

SelectionSort::SelectionSort(int num)
{ 
	setArrayNum(num);
	std::cout << "Generated Array" << std::endl;
	array = GenerateArray(arrayNum);
	PrintArray(array, arrayNum);
}

SelectionSort::~SelectionSort()
{
	DegenArray(array);
}

void SelectionSort::Sort()
{
	if (array != nullptr)
	{
		for (int i = 0; i < arrayNum - 1; i++)
		{
			int indexofsmallest = i;
			for (int j = i + 1; j < arrayNum; j++)
			{
				if (array[j] < array[indexofsmallest])
				{
					indexofsmallest = j;
				}
			}

			int temp = array[i];
			array[i] = array[indexofsmallest];
			array[indexofsmallest] = temp;
		}

		std::cout << std::endl << "After Sort" << std::endl;
		PrintArray(array, arrayNum);
	}
}

void SelectionSort::setArrayNum(int num)
{
	arrayNum = num;
}
