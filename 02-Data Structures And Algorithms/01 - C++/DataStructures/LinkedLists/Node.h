#pragma once
class Node
{
public:
	Node(int Val) { m_value = Val; m_nextNode = nullptr; m_prevNode = nullptr; }
	~Node();
	void PrintNode();
	void SetNodeVal(int);
	int  GetNodeVal();

	void SetPrevNode(Node *);
	void SetNextNode(Node *);

	Node* GetPrevNode();
	Node* GetNextNode();
private:
	int m_value;
	Node* m_nextNode;
	Node* m_prevNode;
};

