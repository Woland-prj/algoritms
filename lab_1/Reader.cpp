#include "Reader.h"

bool reader::IsSpace(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool reader::ReadWord(std::istream& in, std::string& out)
{
	out.clear();
	State state = State::START;
	char ch;
	bool isReadOne = false;

	while (in.get(ch))
	{
		switch (state)
		{
		case State::START:
			if (IsSpace(ch))
			{
				if (ch == '\n')
					state = State::NEW_LINE;
				continue;
			}
			out.push_back(ch);
			if (ch == '-')
				return true;
			isReadOne = true;
			state = State::IN_WORD;
			continue;

		case State::IN_WORD:
			if (ch == '-')
			{
				state = State::AFTER_HYPHEN;
				continue;
			}
			else if (IsSpace(ch))
				return true;
			else
			{
				out.push_back(ch);
				continue;
			}

		case State::AFTER_HYPHEN:
			if (IsSpace(ch) && ch != '\n')
			{
				continue;
			}
			if (ch != '\n')
			{
				out.push_back('-');
				out.push_back(ch);
			}
			state = State::IN_WORD;
			continue;


		case State::NEW_LINE:
			if (ch == '\n')
				return false;
			state = State::START;
		}
	}

	return isReadOne;
}
