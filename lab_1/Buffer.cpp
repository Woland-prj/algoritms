#include "Buffer.h"
#include "Reader.h"
#include <iostream>

buffer::WordsBuffer& buffer::CreateWordsBuffer()
{
	WordsBuffer* buffer = new WordsBuffer(queue::Create(), "",0, 0, false);
	return *buffer;
}

void buffer::PrintFormated(uint16_t lineWidth, std::ostream& out, WordsBuffer& buffer)
{
	uint16_t gapSize;
	uint16_t additionalGap;
	if (buffer.isLast)
	{
		gapSize = 1;
		additionalGap = 0;
	}
	else
	{
		gapSize = buffer.wordsCount > 1 ?
			(lineWidth - buffer.lineWidth) / (buffer.wordsCount - 1) : lineWidth - buffer.lineWidth;
		additionalGap = buffer.wordsCount > 1 ?
			(lineWidth - buffer.lineWidth) % (buffer.wordsCount - 1) : lineWidth - buffer.lineWidth;
	}

	uint16_t filledWidth = 0;
	uint16_t startWordsCount = buffer.wordsCount;
	while (filledWidth < lineWidth && buffer.wordsCount > 1)
	{
		auto word = queue::Pop(buffer.words);
		out << word << std::string(additionalGap > 0 ? gapSize + 1 : gapSize, ' ');
		filledWidth = filledWidth + word.size() + gapSize;
		buffer.wordsCount--;
		if (additionalGap > 0)
			additionalGap--;
	}
	auto lastWord = queue::Pop(buffer.words);
	out << lastWord << std::endl;
	buffer.wordsCount--;
}

void buffer::CollectBuffer(uint16_t lineWidth, std::istream& in, WordsBuffer& buffer)
{
	std::string word;
	uint16_t maxFilledWidth = 0;
	buffer.lineWidth = 0;
	if (!buffer.temp.empty())
	{
		queue::Push(buffer.words, buffer.temp);
		buffer.wordsCount++;
		buffer.lineWidth += buffer.temp.size();
		maxFilledWidth = buffer.temp.size();
	}
	while (reader::ReadWord(in, word))
	{
		buffer.temp = word;
		maxFilledWidth += word.size();
		if (maxFilledWidth + buffer.wordsCount <= lineWidth - 1)
		{
			queue::Push(buffer.words, word);
			buffer.wordsCount++;
			buffer.lineWidth += word.size();
			buffer.temp.clear();
		} else break;
	}

	if (word.empty() )
		buffer.isLast = true;
	else
		buffer.isLast = false;
}
