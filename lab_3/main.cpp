#include "FileSystem.h"
#include <iostream>
#include <fstream>

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
		} else
			fs = new FileSystem("/");
	} catch (const std::exception& ex)
	{
		std::cout << "Ошибка инициализации файловой системы: " << ex.what() << std::endl;
		return 1;
	}
	std::cout << "Прочитанная файловая система:" << std::endl;
	std::cout << *fs << std::endl;
	fs->ResetToRoot();
	fs->Open(0);
	fs->Open(0);
	fs->Open(0);
	fs->Open(0);
	fs->CreateFolder("Some New Subfolder");
	fs->Open(3);
	fs->CreateFile("something.wav");
	fs->MoveUp();
	fs->MoveUp();
	std::cout << "Измененная файловая система:" << std::endl;
	std::cout << *fs << std::endl;
	fs->Cut(0);
	fs->MoveUp();
	fs->MoveUp();
	fs->MoveUp();
	fs->Open(1);
	fs->Paste();
	std::cout << "После перемещения:" << std::endl;
	std::cout << *fs << std::endl;
	fs->Copy(0);
	fs->MoveUp();
	fs->Paste();
	std::cout << "После копирования:" << std::endl;
	std::cout << *fs << std::endl;
	fs->Rename(1, "coreutils");
	fs->Open(1);
	fs->Open(0);
	fs->Rename(1, "bat");
	std::cout << "После переименовывания:" << std::endl;
	std::cout << *fs << std::endl;
	return 0;
}
