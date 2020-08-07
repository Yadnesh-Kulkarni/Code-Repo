#pragma once
class StackItem
{
public:
	StackItem(int value, StackItem* prev) { m_value = value; m_PreviousItem = prev; }

	//Getters
	int GetItemValue();
	StackItem* GetPrevItem();

	//Setters
	void SetItemValue(int);
	void SetPrevItem(StackItem*);
private:
	int m_value;
	StackItem* m_PreviousItem;
};

