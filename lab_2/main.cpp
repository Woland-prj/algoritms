// 24. Имеется  выражение  в  постфиксной  форме,   включающее
// операции '+', '-', '*', '/', '^', ~ (одноместный минус),  SIN,
// COS, EXP.  Переменные  заданы одной строчной латинской буквой.
// Запросить значения переменных и вычислить выражение. Представить
// его в инфиксной форме со скобками. Лишние скобки присутствовать
// не должны (11).
//
// Выполнил: Зимин Н. Ю. ПС-22
// Среда выполнения: MacOS Tahoe 26.0, компилятор - Apple clang version 17.0.0

#include "ExprParser.h"
#include "Reader.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Не указано имя входного файла." << std::endl;
		return 1;
	}

	Reader* reader;
	try
	{
		reader = new Reader(argv[1]);
	} catch (const std::exception& ex)
	{
		std::cerr << "Ошибка инициализации ридера: " << ex.what() << std::endl;
		return 1;
	}
	double res;
	std::string infics;
	try
	{
		auto parser = ExprParser(reader->ReadExpr());
		parser.GetVariablesValues();
		parser.Calc();
		res = parser.GetResult();
		infics = parser.GetInfix();
	} catch (const std::exception& ex)
	{
		std::cerr << "Ошибка вычисления: " << ex.what() << std::endl;
		return 1;
	}
	std::cout << "Результат вычисления: " << infics << "=" << res << std::endl;
	return 0;
}
