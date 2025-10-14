#include "VarMapper.h"
#include <iostream>
#include <limits>
#include <stdexcept>

void VarMapper::DefineVariable(const char c)
{
	CheckChar(c);
	if (!IsDefined(c))
		m_defMap[GetIndex(c)] = true;
}

double VarMapper::GetValue(const char c) const
{
	CheckChar(c);
	if (!IsDefined(c))
		throw std::invalid_argument("Переменная не определена");
	return m_valMap[GetIndex(c)];
}

bool VarMapper::IsDefined(const char c) const
{
	CheckChar(c);
	return m_defMap[GetIndex(c)];
}

void VarMapper::SetDefinedValuesFromCin()
{
	for (uint8_t i = 0; i < alphabetSize; i++)
	{
		if (m_defMap[i])
		{
			while (true)
			{
				std::cout << "Введите значение переменной " << static_cast<char>('a' + i) << ": ";
				double val;
				std::cin >> val;

				if (std::cin.fail())
				{
					std::cin.clear();
					// некорректный ввод, очистка потока
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cerr << "Ошибка: введите число с плавающей точкой!\n";
					continue; // повтор ввода, если не double
				}

				m_valMap[i] = val;
				break; // корректный ввод, следующая переменная
			}
		}
	}
}

uint8_t VarMapper::GetIndex(const char c)
{
	CheckChar(c);
	return c - ASCIIConvConst;
}

void VarMapper::CheckChar(const char c)
{
	if (!IsVariable(c))
		throw std::invalid_argument("Неверный формат переменной");
}

bool VarMapper::IsVariable(const char c)
{
	return c >= 'a' && c <= 'z';
}
