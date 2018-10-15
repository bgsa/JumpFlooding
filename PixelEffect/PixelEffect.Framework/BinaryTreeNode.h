#pragma once

template <typename T>
class BinaryTreeNode
{
public:
	T* parentNode = nullptr;
	T* leftNode = nullptr;
	T* rightNode = nullptr;
		
	~BinaryTreeNode() 
	{
		if (leftNode != nullptr)
			delete leftNode;

		if (rightNode != nullptr)
			delete rightNode;
	}
};

