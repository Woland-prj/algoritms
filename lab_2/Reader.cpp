#include "Reader.h"
#include <fstream>

Reader::Reader(const std::string& fileName)
	: m_in(OpenFile(fileName))
{
}

std::ifstream Reader::OpenFile(const std::string& fileName)
{
	std::ifstream in(fileName);
	if (!in.is_open())
		throw std::runtime_error("Не удалось открыть " + fileName);
	return in;
}

std::string Reader::ReadExpr()
{
	std::string expr;
	std::getline(m_in, expr);
	return expr;
}
