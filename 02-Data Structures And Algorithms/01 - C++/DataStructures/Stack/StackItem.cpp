#include "StackItem.h"

int StackItem::GetItemValue()
{
    return m_value;
}

StackItem* StackItem::GetPrevItem()
{
    return m_PreviousItem;
}

void StackItem::SetItemValue(int value)
{
    m_value = value;
}

void StackItem::SetPrevItem(StackItem* stackItem)
{
    m_PreviousItem = stackItem;
}
