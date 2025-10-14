#ifndef VARMAPPER_H
#define VARMAPPER_H
#include <cstdint>

class VarMapper
{
	constexpr static uint8_t alphabetSize = 26;
	constexpr static uint8_t ASCIIConvConst = 97;

public:
	VarMapper() = default;
	~VarMapper() = default;
	void DefineVariable(char c);
	static bool IsVariable(char c);
	[[nodiscard]] bool IsDefined(char c) const;
	[[nodiscard]] double GetValue(char c) const;
	void SetDefinedValuesFromCin();

private:
	static uint8_t GetIndex(char c);
	static void CheckChar(char c);

	double m_valMap[alphabetSize];
	bool m_defMap[alphabetSize];
};

#endif // VARMAPPER_H
