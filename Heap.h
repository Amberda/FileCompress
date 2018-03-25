#include <iostream>
using namespace std;
#include <stdlib.h>
#include <vector>

//大堆
template<class T>
class MaxHeap
{
public:
	MaxHeap()
	{}
	MaxHeap(T* arr, size_t n)
	{
		_arr.reserve(n);
		//_arr.resize(n);
		for (size_t i = 0; i < n; i++)
		{
			_arr.push_back(arr[i]);
			//_arr[i] = arr[i];
		}
		//建堆
		for (int i = (_arr.size - 2) / 2; i >= 0; --1)
		{
			AdjustDown(i);
		}
	}
	//向下调整
	void AdjustDown(int root)
	{
		int parent = root;
		int child = parent * 2 + 1;//孩子节点下标
		while (child < _arr.size())
		{
			if (child + 1 < _arr.size() && _arr[child + 1] > _arr[child])//找左右孩子中较大的那一个
			{
				++child;
			}
			//判断父亲和孩子的大小
			if (_arr[child] > _arr[parent])
			{
				swap(_arr[parent], _arr[child]);
				parent = child;
				child = parent * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
	//向上调整
	void AdjustUp(int child)
	{
		int parent = (child - 1) / 2;
		while (parent >= 0)
		{
			if (_arr[parent] > _arr[child])
			{
				swap(_arr[parent], _arr[child]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}
	void Push(const T& d)
	{
		_arr.push_back(d);
		AdjustUp(_arr.size() - 1);
	}
	void Pop()
	{
		if (!_arr.empty())
		{
			swap(_arr[0], _arr[_arr.size() - 1]);
			_arr.pop_back();
			AdjustDown(0);
		}
	}
	size_t Size()
	{
		return _arr.size();
	}
	bool Empty()
	{
		return _arr.empty();
	}
	T&Top()const
	{
		return _arr[0];
	}

private:
	vector<T> _arr;
};


//小堆
template<class T>
class MinHeap
{
public:
	MinHeap()
	{}
	MinHeap(T* arr, size_t n)
	{
		_arr.reserve(n);
		for (size_t i = 0; i < n; i++)
		{
			_arr.push_back(arr[i]);
		}
		//建堆
		for (int i = (_arr.size() - 2) / 2; i >= 0; --1)//从倒数第一个非叶子节点开始调整
		{
			AdjustDown(i);
		}
	}
	//向下调整
	void AdjustDown(int root)
	{
		int parent = root;
		int child = parent * 2 + 1;//孩子节点下标
		while (child < _arr.size())
		{
			if (child + 1 < _arr.size() && _arr[child + 1] < _arr[child])//找左右孩子中较小的那一个
			{
				++child;
			}
			//判断父亲和孩子的大小
			if (_arr[child] < _arr[parent])
			{
				swap(_arr[parent], _arr[child]);
				parent = child;
				child = parent * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
	//向上调整
	void AdjustUp(int child)
	{
		int parent = (child - 1) / 2;
		while (parent >= 0)
		{
			if (_arr[parent] < _arr[child])
			{
				swap(_arr[parent], _arr[child]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}
	void Push(const T& d)
	{
		_arr.push_back(d);
		AdjustUp(_arr.size() - 1);
	}
	void Pop()
	{
		if (!_arr.empty())
		{
			swap(_arr[0], _arr[_arr.size() - 1]);
			_arr.pop_back();
			AdjustDown(0);
		}
	}
	size_t Size()
	{
		return _arr.size();
	}
	bool Empty()
	{
		return _arr.empty();
	}
	T& Top()
	{
		return _arr[0];
	}

private:
	vector<T> _arr;
};


//利用仿函数,提高代码的复用率
template <class T>
struct Greater
{
	bool operator()(const T& left, const T& right)
	{
		return left > right;
	}
};
template <class T>
struct Less
{
	bool operator()(const T& left, const T& right)
	{
		return left < right;
	}
};

template<class T, class Compare = Greater<T> >
class Heap
{
public:
	Heap()
	{}
	Heap(const T* arr, size_t n)
	{
		_arr.reserve(n);
		for (size_t i = 0; i < n; i++)
		{
			_arr.push_back(arr[i]);
		}
		//建堆
		for (int i = (_arr.size() - 2) / 2; i >= 0; --i)
		{
			AdjustDown(i);
		}
	}
	//向下调整
	void AdjustDown(int root)
	{
		int parent = root;
		int child = parent * 2 + 1;//孩子节点下标
		Compare com;
		while (child < _arr.size())
		{
			if (child + 1 < _arr.size() && com(_arr[child + 1], _arr[child]))//找左右孩子中较小/大的那一个
			{
				++child;
			}
			//判断父亲和孩子的大小
			if (com(_arr[child], _arr[parent]))
			{
				swap(_arr[parent], _arr[child]);
				parent = child;
				child = parent * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
	//向上调整
	void AdjustUp(int child)
	{
		Compare com;
		int parent = (child - 1) / 2;
		while (parent >= 0) //while (child > 0)
		{
			if (com(_arr[child], _arr[parent]))
			{
				swap(_arr[parent], _arr[child]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}
	void Push(const T& d)
	{
		_arr.push_back(d);
		AdjustUp(_arr.size() - 1);
	}
	void Pop()
	{
		if (!_arr.empty())
		{
			swap(_arr[0], _arr[_arr.size() - 1]);
			_arr.pop_back();
			AdjustDown(0);
		}
	}
	size_t Size()
	{
		return _arr.size();
	}
	bool Empty()
	{
		return _arr.empty();
	}
	T& Top()
	{
		return _arr[0];
	}

private:
	vector<T> _arr;
};

