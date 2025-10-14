#ifndef READER_H
#define READER_H

#include <fstream>
#include <string>
class Reader final
{
public:
	explicit Reader(const std::string& fileName);
	std::string ReadExpr();

private:
	static std::ifstream OpenFile(const std::string& fileName);
	std::ifstream m_in;
};

#endif // READER_H
