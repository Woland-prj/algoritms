#ifndef QUEUE_H
#define QUEUE_H
#include <string>

namespace queue
{

struct Node
{
	std::string value;
	Node* next;
};

struct Queue
{
	Node* head;
	Node* tail;
};

Queue& Create();
void Push(Queue& queue, const std::string& value);
bool IsEmpty(Queue& queue);
std::string Pop(Queue& queue);
void Clear(Queue& queue);

} // namespace queue

#endif // QUEUE_H
