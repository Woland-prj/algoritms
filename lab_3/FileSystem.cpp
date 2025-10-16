#include "FileSystem.h"
#include <istream>
#include <string>
#include <vector>

FileSystem::FileSystem(const std::string& rootName)
	: m_root(std::make_unique<FSNode>(rootName, true, 0, nullptr))
	, m_current(m_root.get())
{
}

FileSystem::FileSystem(std::istream& in)
{
	ParseFile(in);
	m_current = m_root.get();
}

FSNode& FileSystem::GetCurrent() const
{
	return *m_current;
}

void FileSystem::ResetToRoot()
{
	m_current = m_root.get();
}

bool FileSystem::MoveUp()
{
	if (m_current->m_parent != nullptr)
	{
		m_current = m_current->m_parent;
		return true;
	}
	return false;
}

void FileSystem::CheckIndex(const uint16_t childIndex) const
{
	if (childIndex > m_current->m_children.size() - 1)
		throw std::runtime_error(
			std::format(
				"Директория {} имеет {} вложенных элементов. Полученный индекс: {}",
				m_current->m_name, m_current->m_children.size(), childIndex));
}

void FileSystem::Open(const uint16_t childIndex)
{
	CheckIndex(childIndex);
	const auto node = m_current->m_children.at(childIndex).get();
	if (!node->m_isDir)
		throw std::runtime_error(
			std::format(
				"Файл {} не может быть открыт как директория",
				node->m_name));
	m_current = node;
}

void FileSystem::CreateFolder(const std::string& folderName)
{
	if (!m_current->m_isDir)
		throw std::runtime_error(
			std::format("\"{}\" - не директория, невозможно создать директорию \"{}\"", m_current->m_name, folderName));
	m_current->m_children.push_back(
		std::make_unique<FSNode>(folderName, true, m_current->m_level + 1, m_current));
}

void FileSystem::CreateFile(const std::string& fileName)
{
	if (!m_current->m_isDir)
		throw std::runtime_error(
			std::format("\"{}\" - не директория, невозможно создать файл \"{}\"", m_current->m_name, fileName));
	m_current->m_children.push_back(
		std::make_unique<FSNode>(fileName, false, m_current->m_level + 1, m_current));
}

void FileSystem::Copy(const uint16_t childIndex)
{
	CheckIndex(childIndex);
	if (IsCopeing() || IsMoving())
		throw std::runtime_error("Что-то уже скопировано или вырезано");
	m_mode = FSMode::COPY;
	m_interactNode = std::make_unique<FSNode>(*(m_current->m_children.at(childIndex).get()));
}

void FileSystem::Cut(const uint16_t childIndex)
{
	CheckIndex(childIndex);
	if (IsCopeing() || IsMoving())
		throw std::runtime_error("Что-то уже скопировано или вырезано");
	m_mode = FSMode::MOVE;
	m_interactNode = std::move(m_current->m_children.at(childIndex));
	m_current->m_children.erase(m_current->m_children.begin() + childIndex);
}

void FileSystem::Rename(const uint16_t childIndex, const std::string& newName) const
{
	CheckIndex(childIndex);
	m_current->m_children.at(childIndex).get()->m_name = newName;
}

void FileSystem::Paste()
{
	if (!IsCopeing() && !IsMoving())
		throw std::runtime_error("Ничего не скопировано и не вырезано");
	m_interactNode->m_parent = m_current;
	RecalculateLevel(*m_interactNode);
	m_current->m_children.push_back(std::move(m_interactNode));
	m_interactNode = nullptr;
	m_mode = FSMode::NORMAL;
}

void FileSystem::Delete(const uint16_t childIndex) const
{
	CheckIndex(childIndex);
	m_current->m_children.erase(m_current->m_children.begin() + childIndex);
}

void FileSystem::ParseFile(std::istream& in)
{
	std::string currNode;
	int16_t level = -1;
	while (std::getline(in, currNode))
		level = ParseLine(currNode, level);
}

int16_t FileSystem::ParseLine(const std::string& strNode, int16_t prevLevel)
{
	enum class ParseState
	{
		LEVEL,
		NAME,
		PRE_NAME,
		ROOT
	};
	auto state = prevLevel < 0 ? ParseState::ROOT : ParseState::LEVEL;
	int16_t level = 0;
	bool isDir{};
	std::string name;
	for (const auto ch : strNode)
	{
		switch (state)
		{
		case ParseState::LEVEL:
			if (ch == ' ' || ch == '\t')
				break;
			if (ch == '.')
			{
				level++;
				break;
			}
			if (ch != 'D' && ch != 'F')
				throw std::runtime_error(
					std::format("Ожидался тип узла D или F. Прочитано: {} в {}", ch, strNode));
			isDir = ch == 'D';
			state = ParseState::PRE_NAME;
			break;
		case ParseState::PRE_NAME:
			if (ch == ' ' || ch == '\t')
				break;
			state = ParseState::NAME;
		case ParseState::NAME:
			name.push_back(ch);
			break;
		case ParseState::ROOT:
			if (ch == ' ' || ch == '\t')
				break;
			if (ch != 'D')
				throw std::invalid_argument(
					"Корень файловой системы должен начинаться с маркера D! Прочитано: " + strNode);
			isDir = true;
			state = ParseState::PRE_NAME;
			break;
		}
	}
	if (m_root == nullptr)
	{
		m_root = std::make_unique<FSNode>(name, isDir, 0, nullptr);
		m_current = m_root.get();
		return 0;
	}
	if (level - prevLevel > 1)
		throw std::invalid_argument(
			"Спуск по файловой системе ниже уровня детей узла. Прочитано: " + strNode);
	while (level - prevLevel < 1)
	{
		MoveUp();
		prevLevel--;
	}
	if (!m_current->m_isDir)
		throw std::invalid_argument(
			std::format("Невозможно создать узел. {} - не директория Прочитано: {}", m_current->m_name, strNode));
	auto son = std::make_unique<FSNode>(name, isDir, level, m_current);
	FSNode* tmp = son.get();
	m_current->m_children.push_back(std::move(son));
	m_current = tmp;
	return level;
}

void FileSystem::Dump(std::ostream& out) const
{
	Out(out, *m_root);
}

bool FileSystem::IsCopeing() const
{
	return m_mode == FSMode::COPY;
}

bool FileSystem::IsMoving() const
{
	return m_mode == FSMode::MOVE;
}

void FileSystem::Out(std::ostream& out, const FSNode& ptr)
{
	out << ptr;
	for (const auto& son : ptr.m_children)
		Out(out, *son);
}

void FileSystem::RecalculateLevel(const FSNode& ptr)
{
	ptr.m_level = ptr.m_parent->m_level + 1;
	for (const auto& son : ptr.m_children)
		RecalculateLevel(*son);
}

std::ostream& operator<<(std::ostream& out, const FileSystem& fs)
{
	fs.Dump(out);
	return out;
}
