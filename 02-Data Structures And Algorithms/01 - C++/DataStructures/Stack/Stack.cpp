#include "Stack.h"

Stack::Stack(int val)
{
    StackItem* headStackItem = new StackItem(val,nullptr);
    m_TopOfStack = headStackItem;
    m_TotalCount = 1;
}

int Stack::Pop()
{
    if (m_TotalCount == 0)
    {
        StackItem* temp = m_TopOfStack;
        m_TopOfStack = temp->GetPrevItem();
        int retVal = temp->GetItemValue();
        delete temp;
        m_TotalCount--;
        return retVal;
    }
    return -1;
}

void Stack::Push(int value)
{
    StackItem* newItem = new StackItem(value, m_TopOfStack);
    m_TopOfStack = newItem;
    m_TotalCount++;
}
