#include <iostream>
#include "BubbleSort.h"


BubbleSort::BubbleSort()
{
}

BubbleSort::BubbleSort(int num)
{
	setArrayNum(num);

	std::cout << "Generated Array" << std::endl;
	array = GenerateArray(arrayNum);
	PrintArray(array,arrayNum);
}

BubbleSort::~BubbleSort()
{
	DegenArray(array);
}

void BubbleSort::Sort()
{
	if (array != nullptr)
	{
		for (int i = 0; i < arrayNum - 1; i++)
		{
			bool swapped = false;
			for (int j = 0; j + 1 < arrayNum - i; j++)
			{
				if (array[j] > array[j+1])
				{
					swapped = true;
					int temp = array[j+1];
					array[j+1] = array[j];
					array[j] = temp;
				}
			}
			if (!swapped)
			{
				std::cout << "Sorted Array" << std::endl;
				PrintArray(array, arrayNum);
				return;
			}
		}

		std::cout << "Sorted Array" << std::endl;
		PrintArray(array,arrayNum);
	}
}

void BubbleSort::setArrayNum(int num)
{
	arrayNum = num;
}
