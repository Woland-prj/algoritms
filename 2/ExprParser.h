#ifndef EXPRPARSER_H
#define EXPRPARSER_H
#define HIGHEST_PRIORITY 5
#define ZERO_PRIORITY 0
#include "Stack.h"
#include "VarMapper.h"
#include <cstddef>
#include <iostream>
#include <string>

class ExprParser
{
public:
	ExprParser() = delete;
	explicit ExprParser(std::string expr);
	~ExprParser() = default;

	void GetVariablesValues();
	void Calc();
	double GetResult();
	std::string GetInfix();

private:
	struct InfixNode
	{
		std::string expr;
		uint8_t priority;
		bool isRightAssoc;
		bool isFirstUnary = false;
	};

	std::string m_postfixExpr;
	VarMapper m_varMapper{};
	Stack<double> m_calcStack;
	Stack<InfixNode> m_infixExprStack;

	[[nodiscard]] static bool IsUnaryOperation(char c);
	[[nodiscard]] static bool IsBinaryOperation(char c);
	[[nodiscard]] static bool IsFunctionStartChar(char c);

	[[nodiscard]] bool IsSin(size_t fStartPos) const;
	[[nodiscard]] bool IsCos(size_t fStartPos) const;
	[[nodiscard]] bool IsExp(size_t fStartPos) const;
	[[nodiscard]] bool IsFunction(size_t fStartPos) const;
	[[nodiscard]] static bool IsRightAssociative(char op);
	[[nodiscard]] static bool IsCommutare(char op);

	void DoUnaryOperation(char op);
	void DoBinaryOperation(char op);
	void DoFunction(size_t funcStartPos);

	[[nodiscard]] static std::string WrapParensIfNeed(const InfixNode& node, uint8_t parentPriority, bool isRightOperand, bool parentRightAssoc, bool parentCommutare);
	[[nodiscard]] static uint8_t GetOpPriority(char op);
};

#endif // EXPRPARSER_H
