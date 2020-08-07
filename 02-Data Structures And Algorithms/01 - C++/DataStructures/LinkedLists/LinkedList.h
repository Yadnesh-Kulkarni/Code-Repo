#pragma once

class Node;

class LinkedList
{
public:
	LinkedList();
    ~LinkedList();

	//Insertion and deletion operations
	int InsertValue(int); // Default Will be appended at the end
	int InsertValue(int, int); // First parameter is value of node // second parameter is after which node, new value should be inserted
	int RemoveValue(int); // If multiple entries do exist then all of the entries with value as parameter will be deleted
	
	//Printing the complete list
	void PrintLinkedList();

	//Utilities
	bool IsEmpty();
	bool SearchValue(int);

private:

	Node* m_StartNode;
	Node* m_EndNode;
	
	int m_TotalNodeCount;

	void SetStartNode(Node*);
	void SetEndNode(Node*);

	Node* GetStartNode();
	Node* GetEndNode();


};

