#ifndef BUFFER_H
#define BUFFER_H
#include "Queue.h"
#include <iostream>

namespace buffer
{

using WordsBuffer = struct
{
	queue::Queue& words;
	std::string temp;
	uint16_t lineWidth;
	uint16_t wordsCount;
	bool isLast;
};

WordsBuffer& CreateWordsBuffer();
void PrintFormated(uint16_t lineWidth, std::ostream& out, WordsBuffer& buffer);
void CollectBuffer(uint16_t lineWidth, std::istream& in, WordsBuffer& buffer);

} // namespace buffer

#endif // BUFFER_H
