#pragma once

#include "BinaryTreeNode.h"

template <typename NodeType>
class BinaryTree
{
public:
	NodeType * root;
	
	~BinaryTree()
	{
		if (root != nullptr)
			delete root;
	}
};

