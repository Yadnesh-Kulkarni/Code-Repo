#include <iostream>
#include "Tree.h"

int main()
{
    Tree* bTree = new Tree(5);

    std::cout << bTree->GetRootNode()->GetNodeVal() << std::endl;

    delete bTree;
}
