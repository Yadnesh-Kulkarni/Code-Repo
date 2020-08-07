#include <iostream>

#include "SelectionSort.h"
#include "BubbleSort.h"
#include "MergeSort.h"

int main(int argc, char** argv)
{
	void UseSelectionSort();
	void UseBubbleSort();
	void UseMergeSort();

	int algoChoice = -1;
	bool bQuit = false;

	while (bQuit == false)
	{
		std::cout << "Please Select Algorithm " << std::endl << "1. Selection Sort" << std::endl << "2. Bubble Sort" << std::endl << "3. Bubble Optimized Sort" << std::endl << "4. Merge Sort" << std::endl << "5. Quick Sort" << std::endl << "6. Quick Median Sort" << std::endl;
		std::cout << "0. Quit" << std::endl;
		std::cin >> algoChoice;

		switch (algoChoice)
		{
		case 1:
			UseSelectionSort();
			break;
		case 2:
			UseBubbleSort();
			break;
		case 3:
			UseBubbleSort();
			break;
		case 4:
			UseMergeSort();
			break;
		case 5:
			std::cout << "Quick Sort" << std::endl;
			break;
		case 6:
			std::cout << "Quick Median Sort" << std::endl;
			break;
		case 0:
			bQuit = true;
			break;
		default:
			std::cout << "Invalid Choice" << std::endl; 
			break;
		}
	}

	return 0;
}

void UseSelectionSort()
{
	std::cout << "Applying Selection Sort" << std::endl;
	SelectionSort SS(6);
	SS.Sort();
}

void UseBubbleSort()
{
	std::cout << "Applying Bubble Sort" << std::endl;
	BubbleSort BS(5);
	BS.Sort();
}

void UseMergeSort()
{
	std::cout << "Merge Sort" << std::endl;
	MergeSort MSort(5);
	MSort.Sort();
}