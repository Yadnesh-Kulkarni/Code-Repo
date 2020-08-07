#pragma once

typedef struct {
    int m_value;
    TreeNode* m_leftChild;
    TreeNode* m_rightChild;

    int GetNodeVal() { return m_value; }
    TreeNode* GetLeftChild() { return m_leftChild; }
    TreeNode* GetRightChild() { return m_rightChild; }

    void SetNodeVal(int num) { m_value = num; }
    void SetLeftChild(TreeNode* node) { m_leftChild = node; }
    void SetRightChild(TreeNode* node) { m_rightChild = node; }
}TreeNode;

class Tree 
{
public:
    Tree(int num) { m_RootNode->m_value = num; m_RootNode->m_rightChild = nullptr; m_RootNode->m_leftChild = nullptr; }

    //Insertion Operation

    //Deletion Operation

    //Traverse Tree

    //GettersAndSetters
    TreeNode* GetRootNode() { return m_RootNode; }

private :

    TreeNode* m_RootNode;
};

