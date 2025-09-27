#ifndef READER_H
#define READER_H
#include <iostream>
#include <string>

namespace reader
{

enum class State
{
	START, // начало чтения слова
	IN_WORD, // внутри слова
	AFTER_HYPHEN, // после дефиса (проверить есть ли перенос)
	NEW_LINE // после \n
};

bool IsSpace(char c);
bool ReadWord(std::istream& in, std::string& out);

} // namespace reader

#endif // READER_H
