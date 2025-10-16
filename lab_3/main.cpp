#include "FileSystem.h"
#include "TUI.h"
#include <fstream>
#include <iostream>

std::ifstream OpenFile(const std::string& fileName)
{
	std::ifstream in(fileName);
	if (!in.is_open())
		throw std::runtime_error("Не удалось открыть " + fileName);
	return in;
}

int main(int argc, char* argv[])
{
	FileSystem* fs;
	try
	{
		if (argc > 1)
		{
			std::ifstream in = OpenFile(argv[1]);
			fs = new FileSystem(in);
		}
		else
			fs = new FileSystem("/");
	}
	catch (const std::exception& ex)
	{
		std::cout << "Ошибка инициализации файловой системы: " << ex.what() << std::endl;
		return 1;
	}
	TUI tui(*fs);
	tui.Run();
	return 0;
}
