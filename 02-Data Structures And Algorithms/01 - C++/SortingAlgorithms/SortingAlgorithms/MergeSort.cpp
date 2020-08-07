#include <iostream>
#include "MergeSort.h"

MergeSort::MergeSort(int num)
{
	setArrayNum(num);

	//Generate Array In Constructor Itself
	std::cout << "Generated Array" << std::endl;
	array = GenerateArray(arrayNum);
	PrintArray(array,arrayNum);
}

MergeSort::~MergeSort()
{
	DegenArray(array);
}

void MergeSort::setArrayNum(int num)
{
	arrayNum = num;
}

void MergeSort::Sort()
{
	//First Pass Complete Array to Function
	//For the sake of parity and recursion
	//Can be adjusted to directly access array in function itself (In that case will need to define one more function)
	DivideArrayInTwo(0,arrayNum - 1);

	std::cout << "After Sort" << std::endl;

	PrintArray(array,arrayNum);
}

void MergeSort::DivideArrayInTwo(int start,int end)
{
	//Check weather beginning index is less than ending
	if (start < end)//The recursive condition
	{
		int middle = (start + end) / 2;
		DivideArrayInTwo(start , middle);//Recursive Call for Left Subpart
		DivideArrayInTwo(middle + 1,end);// Recursive Call for Right Subpart
		MergeArrays(start,middle,end);// Final Merge Call to Merge above two arrays
	}
}

void MergeSort::MergeArrays(int from,int middle,int to)
{
	//Calculate Left and Right Subpart lengths so that new arrays can be allocated
	int LeftSubpartLength = middle - from + 1;// Plus 1 is allocated because middle element is going to be counted as leftsubpart
	int RightSubpartLength = to - middle;
	int* left = new int[LeftSubpartLength + 1];//Plus 1 is allocated to solve runtime bugs while testing
	int* right = new int[RightSubpartLength + 1];

	for (int i = 0; i < LeftSubpartLength; i++)
	{
		left[i] = array[from + i];//Copying left array
	}

	for (int i = 0; i < RightSubpartLength; i++)
	{
		right[i] = array[middle + i + 1];//Copying right array // Plus 1 is copied as right array begins at middle + 1
	}

	left[LeftSubpartLength] = INT_MAX;//For checking array bounds
	right[RightSubpartLength] = INT_MAX;//For checking array bounds

	int LeftSubpartPointer = 0;//Left Array Pointer
	int RightSubpartPointer = 0;//Right Array Pointer

	//Loop goes for entire array from start to end
	for (int i = from; i <= to; i++)
	{
		//If element pointed by LeftPointer is greater than pointed by right
		if (left[LeftSubpartPointer] > right[RightSubpartPointer])
		{
			array[i] = right[RightSubpartPointer];//copy lower value to ith element in main array
			RightSubpartPointer++;
		}
		else
		{
			array[i] = left[LeftSubpartPointer];//copy lower element to ith element in main array
			LeftSubpartPointer++;
		}
	}
}
