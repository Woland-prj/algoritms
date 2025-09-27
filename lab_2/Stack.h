#ifndef STACK_H
#define STACK_H

#include <cstddef>
template <typename T>
class Stack final
{
	struct Node
	{
		T data;
		Node* next;
		Node(const T& d, Node* n)
			: data(d)
			, next(n)
		{
		}
	};

public:
	Stack() = default;

	~Stack()
	{
		while (!IsEmpty())
			Pop();
	}

	void Push(T data)
	{
		Node* node = new Node(data, nullptr);
		m_size++;
		if (IsEmpty())
		{
			m_top = node;
			return;
		}
		node->next = m_top;
		m_top = node;
	}

	T* Top()
	{
		if (IsEmpty())
			return nullptr;
		return &m_top->data;
	}

	void Pop()
	{
		if (!IsEmpty())
		{
			Node* node = m_top;
			m_top = node->next;
			delete node;
			m_size--;
		}
	}

	bool IsEmpty()
	{
		return m_top == nullptr;
	}

	[[nodiscard]] size_t GetSize() const
	{
		return m_size;
	}

private:
	Node* m_top = nullptr;
	size_t m_size = 0;
};

#endif // STACK_H
