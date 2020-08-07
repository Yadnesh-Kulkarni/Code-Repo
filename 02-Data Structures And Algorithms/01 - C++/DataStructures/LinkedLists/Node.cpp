#include "Node.h"

#include <iostream>

Node::~Node()
{
	m_value = -1;
	m_nextNode = nullptr;
	m_prevNode = nullptr;
}

void Node::PrintNode()
{
	std::cout << this->m_value << std::endl;
}

void Node::SetNodeVal(int Value)
{
	m_value = Value;
}

int Node::GetNodeVal()
{
	return m_value;
}

void Node::SetPrevNode(Node *prevNode)
{	
	m_prevNode = prevNode;
}

void Node::SetNextNode(Node *nextNode)
{
	m_nextNode = nextNode;
}

Node* Node::GetPrevNode()
{
	return m_prevNode;
}

Node* Node::GetNextNode()
{
	return m_nextNode;
}
