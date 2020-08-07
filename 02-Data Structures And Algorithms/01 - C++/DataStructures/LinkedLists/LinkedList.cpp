#include "LinkedList.h"
#include "Node.h"
#include <iostream>

LinkedList::LinkedList()
{
    Node* tempNode = new Node(-1);
    SetStartNode(tempNode);
    SetEndNode(tempNode);
    m_TotalNodeCount = 0;

    tempNode = nullptr;
}

LinkedList::~LinkedList()
{
    if (!IsEmpty())
    {
        Node* traversePtr;
        traversePtr = GetEndNode();

        while (traversePtr != nullptr)
        {
            Node* tempNode = traversePtr;
            traversePtr = tempNode->GetPrevNode();

            delete tempNode;
        }
    }
}

int LinkedList::InsertValue(int value)
{
    Node* tempNode = GetEndNode();

    if (!IsEmpty())
    {
        Node* newNode = new Node(value);
        newNode->SetPrevNode(tempNode);
        tempNode->SetNextNode(newNode);

        SetEndNode(newNode);
    }
    else
    {
        Node* tempFirstNode = GetStartNode();
        Node* newNode = new Node(value);
        newNode->SetPrevNode(tempNode);
        tempNode->SetNextNode(newNode);

        SetEndNode(newNode);
        SetStartNode(newNode);

        delete tempFirstNode;
    }
    m_TotalNodeCount++;
    return 0;
}

int LinkedList::InsertValue(int value, int after)
{
    if (!IsEmpty())
    {
        Node* tempNode = GetStartNode();
        while (tempNode != nullptr)
        {
            if (after == tempNode->GetNodeVal())
            {
                Node* newNode = new Node(value);
                Node* nodeToShift = tempNode->GetNextNode();
                
                tempNode->SetNextNode(newNode);
                newNode->SetNextNode(nodeToShift);

                nodeToShift->SetPrevNode(newNode);
                newNode->SetPrevNode(tempNode);
            }
        }
    }
    else
    {
        std::cout << "Invalid Call" << std::endl;
    }
    return 0;
}

int LinkedList::RemoveValue(int value)
{
    if (!IsEmpty())
    {
        Node* tempNode = GetStartNode();
        while (tempNode != nullptr)
        {
            if (value == tempNode->GetNodeVal())
            {
                Node* PrevNode = (tempNode->GetPrevNode() == nullptr) ? nullptr : tempNode->GetPrevNode();
                Node* NextNode = (tempNode->GetNextNode() == nullptr) ? nullptr : tempNode->GetNextNode();

                if (PrevNode != nullptr)
                {
                    PrevNode->SetNextNode(NextNode);
                }
                else
                {
                    SetStartNode(NextNode);
                }


                if (NextNode != nullptr)
                {
                    NextNode->SetPrevNode(PrevNode);
                }
                else
                {
                    SetEndNode(PrevNode);
                }

                delete tempNode;
                tempNode = PrevNode;
            }
            tempNode = tempNode->GetNextNode();
        }
    }

    return 0;
}

void LinkedList::PrintLinkedList()
{
    if (IsEmpty())
    {
        std::cout << "List is empty" << std::endl;
    }
    else
    {
        Node* nodePtr = GetStartNode();
        int i = 0;
        while (nodePtr != nullptr)
        {
            std::cout << i << " : " << nodePtr->GetNodeVal() << std::endl;
            nodePtr = nodePtr->GetNextNode();
            i++;
        }
    }
}

bool LinkedList::IsEmpty()
{
    if (m_StartNode == m_EndNode && m_TotalNodeCount == 0)
    {
        return true;
    }
    return false;
}

bool LinkedList::SearchValue(int)
{
    return false;
}

void LinkedList::SetStartNode(Node *startNode)
{
    m_StartNode = startNode;
    m_StartNode->SetPrevNode(nullptr);
}

void LinkedList::SetEndNode(Node *endNode)
{
    m_EndNode = endNode;
    m_EndNode->SetNextNode(nullptr);
}

Node* LinkedList::GetStartNode()
{
    return m_StartNode;
}

Node* LinkedList::GetEndNode()
{
    return m_EndNode;
}
