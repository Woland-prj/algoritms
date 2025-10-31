// 26. Информация  о  файлах  на  жестких  дисках   компьютера
// записана  с  помощью  дерева.  Обеспечить выполнение следующих
// операций:
//    1) загрузку дерева в память из файла;
//    2) обход дерева папок в  режиме  диалога  (раскрытие папок,
//       подъем на уровень и т. п.);
//    3) корректировку  дерева при создании новых папок и файлов,
// их переименовании, копировании, переносе и удалении.
//    4) сохранение дерева в файле (13).
//
// Выполнил: Зимин Никита, ПС-22
// Среда выполнения MacOS 26.0 Tahoe, Apple clang version 17.0.0
//
#include "FileSystem.h"
#include "TUI.h"
#include <fstream>
#include <iostream>

std::fstream OpenFile(const std::string& fileName)
{
	std::fstream f(fileName);
	if (!f.is_open())
		throw std::runtime_error("Не удалось открыть " + fileName);
	return f;
}

int main(int argc, char* argv[])
{
	std::fstream f;
	if (argc > 1)
	{
		try
		{
			f = OpenFile(argv[1]);
		}
		catch (const std::exception& ex)
		{
			std::cout << "Ошибка открытия файла: " << ex.what() << std::endl;
		}
	}
	FileSystem fs = f.is_open() ? FileSystem(f) : FileSystem();
	TUI tui(fs);
	tui.Run();
	if (f.is_open())
	{
		f.close();
		std::ofstream out(argv[1], std::ios::trunc);
		out << fs;
		out.close();
	}
	return 0;
}
