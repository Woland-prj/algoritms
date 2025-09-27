// 24. В текстовом файле записан  отдельный  абзац.  Некоторые
// слова перенесены со строки на следующую строку.  Знак переноса
// '-'.  Выровнять строки абзаца по ширине. Иными словами, правые
// границы  строк  выравниваются  по  заданной  позиции  за  счет
// вставки дополнительных пробелов между словами.  Первая  строка
// абзаца должна иметь заданный отступ, а остальные строки должны
// начинаться  с  первой  позиции.  Последняя  строка  абзаца  по
// правому  краю  не  выравнивается. Переносы в выходном файле не
// допускаются.  Число  строк в исходном и сконечном файлах может
// отличаться (9).

// Выполнил - Зимин Никита, ПС-22
// Среда выполнения - MacOS 26, Apple clang version 17.0.0

#include "Buffer.h"
#include <fstream>
#include <iostream>

using IO = struct
{
	std::ifstream in;
	std::ofstream out;
};

IO Open(const std::string& src, const std::string& dest)
{
	IO io{std::ifstream(src, std::ios::in), std::ofstream(dest, std::ios::out)};
	if (!io.in.is_open()) {
		throw std::runtime_error("Не удалось открыть файл для чтения: " + src);
	}
	if (!io.out.is_open()) {
		io.in.close();
		throw std::runtime_error("Не удалось открыть файл для записи: " + dest);
	}
	return io; // RVO, по значению
}

void Close(IO& io)
{
	io.in.close();
	io.out.close();
}

uint16_t ParseInt(const std::string& str)
{
	constexpr uint16_t max = (1u << (8 * sizeof(uint16_t))) - 1;
	uint16_t result = 0;
	for (char c : str)
	{
		if (c < '0' || c > '9')
			throw std::runtime_error("Символ " + std::string(1, c) + " не цифра в последовательности: " + str);
		if (result > (max - (c - '0')) / 10)
			throw std::runtime_error("Переполнение: " + str);
		result = result * 10 + (c - '0');
	}
	return result;
}

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		std::cerr << "Неверные число аргументов" << std::endl;
		return 1;
	}
	IO io;
	uint16_t indent;
	uint16_t lineWidth;
	try
	{
		indent = ParseInt(std::string(argv[3]));
		lineWidth = ParseInt(std::string(argv[4]));
		if (indent >= lineWidth / 2)
		{
			std::cerr << "Отступ больше половины ширины строки. Форматирование невозможно." << std::endl;
			return 1;
		}
		io = Open(argv[1], argv[2]);
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Ошибка выполнения: " << ex.what() << std::endl;
		return 1;
	}
	io.out << std::string(indent, ' ');
	auto buff = buffer::CreateWordsBuffer();
	buffer::CollectBuffer(lineWidth - indent, io.in, buff);
	buffer::PrintFormated(lineWidth - indent, io.out, buff);
	while (!buff.isLast)
	{
		buffer::CollectBuffer(lineWidth, io.in, buff);
		buffer::PrintFormated(lineWidth, io.out, buff);
	}
	std::cout << "Форматирование успешно!" << std::endl;
	Close(io);
	return 0;
}
