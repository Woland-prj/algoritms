#include "Queue.h"
#include <string>

queue::Queue& queue::Create()
{
	Queue* queue = new Queue(nullptr, nullptr);
	return *queue;
}

void queue::Push(Queue& queue, const std::string& value)
{
	Node* node = new Node();
	node->value = value;
	node->next = nullptr;
	if (queue.tail == nullptr)
	{
		queue.head = node;
		queue.tail = node;
	}
	else
	{
		queue.tail->next = node;
		queue.tail = node;
	}
}

bool queue::IsEmpty(Queue& queue)
{
	return queue.head == nullptr;
}

std::string queue::Pop(Queue& queue)
{
	if (IsEmpty(queue))
	{
		return "";
	}
	std::string value = queue.head->value;
	Node* node = queue.head;
	queue.head = queue.head->next;
	if (queue.head == nullptr)
		queue.tail = nullptr;
	delete node;
	return value;
}

void queue::Clear(Queue& queue)
{
	while (!IsEmpty(queue))
	{
		Pop(queue);
	}
}
