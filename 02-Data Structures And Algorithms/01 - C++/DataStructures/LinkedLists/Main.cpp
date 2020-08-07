#include <iostream>
#include "LinkedList.h"

int main()
{
	LinkedList *list = new LinkedList();

	list->InsertValue(5);
	list->InsertValue(10);
	list->InsertValue(15);
	list->InsertValue(20);
	list->InsertValue(25);
	list->InsertValue(30);
	
	std::cout << "\n\n\nFirst Print\n";
	list->PrintLinkedList();

	list->RemoveValue(20);

	std::cout << "\n\n\nSecond Print\n";
	list->PrintLinkedList();

	list->InsertValue(15);

	std::cout << "\n\n\nThird Print\n";
	list->PrintLinkedList();

	list->RemoveValue(15);

	std::cout << "\n\n\nLast Print\n";
	list->PrintLinkedList();

	delete list;

	return 0;
}