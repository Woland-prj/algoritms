// 24. Имеется сеть автомобильных дорог с двусторонним движением.
// Известна длина каждой дороги.  Принято  решение  о  прекращении
// дальнейшего ремонта и последующего  закрытия  некоторых  дорог.
// При этом для каждой  пары  сообщающихся  ранее  городов  должна
// оставаться возможность связи. Найти такой перечень закрывающихся
// дорог, чтобы их общая длина была  максимальна.  Определить  эту
// длину (12).
//
// Выполнил: Зимин Никита ПС-22
// Среда выполнения: MacOS 26.1 Tahoe, Apple clang version 17.0.0
#include "MST.h"
#include "Structs.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Ошибка: укажите путь к файлу с графом." << std::endl;
		return 1;
	}

	const char* filePath = argv[1];
	std::ifstream graphFile(filePath);

	if (!graphFile.is_open())
	{
		std::cerr << "Ошибка: не удалось открыть файл \"" << filePath << "\"." << std::endl;
		return 1;
	}

	try
	{
		structs::WeightedGraph graph(graphFile);
		auto mst = mst::KruskalAlg(graph);
		std::cout << "Список закрытых дорог:\n"
				  << graph.GetEdges() - mst.GetEdges();

		std::cout << "Длина закрытых дорог: "
		          << graph.GetWeight() - mst.GetWeight() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Ошибка при обработке графа: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
