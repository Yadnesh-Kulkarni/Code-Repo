#include "SortBase.h"
#include <iostream>
#include <stdlib.h>

int* SortBase::GenerateArray(int length)
{
    int* arrayPtr;
    arrayPtr = new int[length];
    for (int i = 0; i < length; i++) {
        arrayPtr[i] = 100 + (rand() % static_cast<int>(901));
    }

    return arrayPtr;
}

void SortBase::DegenArray(int* array)
{
    delete[] array;
}

void SortBase::PrintArray(int* array, int length)
{
    for (int i = 0; i < length; i++)
    {
        std::cout << array[i] << "\t";
    }

    std::cout << std::endl << std::endl;
}

void SortBase::Sort()
{

}
