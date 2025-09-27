#include "ExprParser.h"
#include <format>
#include <string>

ExprParser::ExprParser(std::string expr)
	: m_postfixExpr(std::move(expr))
{
	if (m_postfixExpr.empty())
		throw std::invalid_argument("Не найдено выражение для рассчета");
	m_varMapper = VarMapper();
	size_t i = 0;
	while (i < m_postfixExpr.size())
	{
		if (IsFunction(i))
		{
			i += 3;
			continue;
		}
		const char c = m_postfixExpr[i];
		if (VarMapper::IsVariable(c))
		{
			m_varMapper.DefineVariable(c);
			i++;
			continue;
		}
		if (!IsBinaryOperation(c) && !IsUnaryOperation(c))
			throw std::invalid_argument("Синтаксическая ошибка в позиции " + std::to_string(i + 1));
		i++;
	}
};

void ExprParser::GetVariablesValues()
{
	m_varMapper.SetDefinedValuesFromCin();
}

void ExprParser::Calc()
{
	size_t i = 0;
	while (i < m_postfixExpr.size())
	{
		const char c = m_postfixExpr[i];
		if (VarMapper::IsVariable(c))
		{
			m_calcStack.Push(m_varMapper.GetValue(c));
			m_infixExprStack.Push({std::format("{}", m_varMapper.GetValue(c)), ZERO_PRIORITY, false});
			i++;
			continue;
		}
		if (IsUnaryOperation(c))
		{
			DoUnaryOperation(c);
			i++;
			continue;
		}
		if (IsBinaryOperation(c))
		{
			DoBinaryOperation(c);
			i++;
			continue;
		}
		if (IsFunction(i))
		{
			DoFunction(i);
			i += 3;
			continue;
		}
		throw std::runtime_error("Синтаксическая ошибка в позиции " + std::to_string(i + 1));
	}
}

double ExprParser::GetResult()
{
	if (m_calcStack.IsEmpty())
		return 0;
	const double res = *m_calcStack.Top();
	m_calcStack.Pop();
	return res;
}

bool ExprParser::IsUnaryOperation(const char c)
{
	return c == '~';
}

bool ExprParser::IsBinaryOperation(const char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool ExprParser::IsFunctionStartChar(const char c)
{
	return c == 'S' || c == 'C' || c == 'E';
}

bool ExprParser::IsSin(const size_t fStartPos) const
{
	return m_postfixExpr[fStartPos] == 'S' && m_postfixExpr[fStartPos + 1] == 'I' && m_postfixExpr[fStartPos + 2] == 'N';
}

bool ExprParser::IsCos(const size_t fStartPos) const
{
	return m_postfixExpr[fStartPos] == 'C' && m_postfixExpr[fStartPos + 1] == 'O' && m_postfixExpr[fStartPos + 2] == 'S';
}

bool ExprParser::IsExp(const size_t fStartPos) const
{
	return m_postfixExpr[fStartPos] == 'E' && m_postfixExpr[fStartPos + 1] == 'X' && m_postfixExpr[fStartPos + 2] == 'P';
}

bool ExprParser::IsFunction(const size_t fStartPos) const
{
	return IsSin(fStartPos) || IsCos(fStartPos) || IsExp(fStartPos);
}

void ExprParser::DoUnaryOperation(const char op)
{
	if (m_calcStack.GetSize() < 1 || m_infixExprStack.GetSize() < 1)
		throw std::invalid_argument("Недостаточно операндов для выполнения операции " + std::string(1, op));
	const double a = *m_calcStack.Top();
	m_calcStack.Pop();
	const InfixNode infA = *m_infixExprStack.Top();
	m_infixExprStack.Pop();
	double result;
	std::string expr;
	const uint8_t priority = GetOpPriority(op);
	const bool rightAssoc = IsRightAssociative(op);
	switch (op)
	{
	case '~':
		result = -a;
		expr = "-" + WrapParensIfNeed(infA, priority, false, rightAssoc, false);
		break;
	default:
		throw std::invalid_argument("Неизвестная операция" + std::string(1, op));
	}
	m_calcStack.Push(result);
	m_infixExprStack.Push({ expr, priority, rightAssoc });
}

void ExprParser::DoBinaryOperation(const char op)
{
	if (m_calcStack.GetSize() < 2 || m_infixExprStack.GetSize() < 2)
		throw std::invalid_argument("Недостаточно операндов для выполнения операции " + std::string(1, op));
	const double b = *m_calcStack.Top();
	m_calcStack.Pop();
	const double a = *m_calcStack.Top();
	m_calcStack.Pop();
	const InfixNode right = *m_infixExprStack.Top();
	m_infixExprStack.Pop();
	const InfixNode left = *m_infixExprStack.Top();
	m_infixExprStack.Pop();
	double result;
	std::string expr;
	const uint8_t priority = GetOpPriority(op);
	const bool rightAssoc = IsRightAssociative(op);
	const bool commutare = IsCommutare(op);
	switch (op)
	{
	case '+':
		result = a + b;
		break;
	case '-':
		result = a - b;
		break;
	case '*':
		result = a * b;
		break;
	case '/':
		result = a / b;
		break;
	case '^':
		result = std::pow(a, b);
		break;
	default:
		throw std::invalid_argument("Неизвестная операция " + std::string(1, op));
	}
	const std::string leftExpr = WrapParensIfNeed(left, priority, false, rightAssoc, commutare);
	const std::string rightExpr = WrapParensIfNeed(right, priority, true, rightAssoc, commutare);

	m_calcStack.Push(result);
	m_infixExprStack.Push({leftExpr + op + rightExpr, priority, rightAssoc});
}

void ExprParser::DoFunction(const size_t funcStartPos)
{
	if (m_calcStack.GetSize() < 1 || m_infixExprStack.GetSize() < 1)
		throw std::invalid_argument("Недостаточно операндов для выполнения функции");
	const double a = *m_calcStack.Top();
	m_calcStack.Pop();
	const InfixNode arg = *m_infixExprStack.Top();
	m_infixExprStack.Pop();
	double result;
	std::string funcName;
	if (IsSin(funcStartPos)) {
		result = std::sin(a);
		funcName = "SIN";
	} else if (IsCos(funcStartPos)) {
		result = std::cos(a);
		funcName = "COS";
	} else if (IsExp(funcStartPos)) {
		result = std::exp(a);
		funcName = "EXP";
	} else {
		throw std::runtime_error("Неизвестная функция");
	}
	const std::string expr = funcName + "(" + arg.expr + ")";
	m_calcStack.Push(result);
	m_infixExprStack.Push({expr, HIGHEST_PRIORITY, false});
}
std::string ExprParser::WrapParensIfNeed(const InfixNode& node, const uint8_t parentPriority, const bool isRightOperand, const bool parentRightAssoc, const bool parentCommutare)
{
	bool needParens = false;
	if (node.priority < parentPriority && node.priority != ZERO_PRIORITY)
	{
		needParens = true;
	}
	else if (!parentCommutare && node.priority == parentPriority)
	{
		if (parentRightAssoc)
		{
			// правая ассоциативность, типо (a^b)^c
			if (!isRightOperand)
				needParens = true;
		}
		else
		{
			// левая ассоциативность, типо a-(b-c)
			if (isRightOperand)
			{
				needParens = true;
			}
		}
	}
	return needParens ? "(" + node.expr + ")" : node.expr;
}

uint8_t ExprParser::GetOpPriority(const char op)
{
	switch (op)
	{
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	case '^':
		return 3;
	case '~':
		return 4;
	default:
		return ZERO_PRIORITY;
	}
}

bool ExprParser::IsRightAssociative(const char op)
{
	return op == '^' || op == '~';
}

bool ExprParser::IsCommutare(const char op)
{
	return op == '*' || op == '+';
}

std::string ExprParser::GetInfix()
{
	std::string result;
	if (m_infixExprStack.GetSize() > 1)
	{
		const std::string right = m_infixExprStack.Top()->expr;
		m_infixExprStack.Pop();
		const std::string left = m_infixExprStack.Top()->expr;
		m_infixExprStack.Pop();
		throw std::runtime_error("Не найдено операции для операндов " + left + " и " + right);
	}
	while (!m_infixExprStack.IsEmpty())
	{
		result += m_infixExprStack.Top()->expr;
		m_infixExprStack.Pop();
	}
	return result;
}