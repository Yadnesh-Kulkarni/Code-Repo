#pragma once
class TreeNode
{
public:
	TreeNode() {  }
	TreeNode(int num) { m_value = num; m_left = nullptr; m_right = nullptr; }
	TreeNode(int num, TreeNode* left, TreeNode* right) { m_value = num; m_left = left; m_right = right; }

private:
	int m_value;
	TreeNode* m_left;
	TreeNode* m_right;
};

