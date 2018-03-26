#pragma once

#include "Heap.h"


template <class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const W& weight)
	:_pLeft(NULL)
	, _pRight(NULL)
	, _pParent(NULL)
	, _weight(weight)
	{}

	HuffmanTreeNode<W>* _pLeft;
	HuffmanTreeNode<W>* _pRight;
	HuffmanTreeNode<W>* _pParent;
	W _weight;
};

template <class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W>* pNode;
public:
	HuffmanTree()
		:_pRoot(NULL)
	{}
	HuffmanTree(W* array, size_t size, const W& invalid)
	{
		_CreateHuffmanTree(array, size, invalid);
	}
	~HuffmanTree()
	{
		_Destroy(_pRoot);
	}
	pNode GetRoot()
	{
		return _pRoot;
	}
private:
	void _CreateHuffmanTree(W* array, size_t size, const W& invalid)
	{
		struct Compare
		{
			bool operator()(pNode pLeft, pNode pRight)
			{
				return pLeft->_weight < pRight->_weight;
			}
		};
		Heap<pNode, Compare> hp;
		for (size_t i = 0; i < size; i++)
		{
			if (array[i] != invalid)
			{
				hp.Push(new HuffmanTreeNode<W>(array[i]));
			}
		}

		if (hp.Empty())
		{
			_pRoot = NULL;
		}

		while (hp.Size()>1)
		{
			pNode pLeft = hp.Top();
			hp.Pop();
			pNode pRight = hp.Top();
			hp.Pop();

			pNode pParent = new HuffmanTreeNode<W>(pLeft->_weight + pRight->_weight);
			pParent->_pLeft = pLeft;
			pLeft->_pParent = pParent;
			pParent->_pRight = pRight;
			pRight->_pParent = pParent;

			hp.Push(pParent);
		}
		_pRoot = hp.Top();
	}
	void _Destroy(pNode& pRoot)
	{
		if (pRoot)
		{
			_Destroy(pRoot->_pRight);
			_Destroy(pRoot->_pLeft);
			delete pRoot;
			pRoot = NULL;
		}
	}
private:
	pNode _pRoot;
};
