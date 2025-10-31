#include "FSNode.h"
#include <ostream>

FSNode::FSNode(std::string name, const bool isDir, const int16_t level, FSNode* parent)
	: m_name(std::move(name))
	, m_isDir(isDir)
	, m_parent(parent)
	, m_level(level) {};

FSNode::FSNode(const FSNode& other)
	: m_name(other.m_name)
	, m_isDir(other.m_isDir)
	, m_parent(nullptr)
	, m_level(other.m_level)
{
	m_children.reserve(other.m_children.size());
	for (const auto& child : other.m_children)
	{
		auto copyChild = std::make_unique<FSNode>(*child);
		copyChild->m_parent = this;
		m_children.push_back(std::move(copyChild));
	}
}

std::string FSNode::GetName() const
{
	return m_name;
}

bool FSNode::IsDir() const
{
	return m_isDir;
}

int16_t FSNode::GetLevel() const
{
	return m_level;
}

std::vector<FSNode*> FSNode::GetChildren() const
{
	std::vector<FSNode*> res;
	res.reserve(m_children.size());
	for (const auto& child : m_children)
		res.push_back(child.get());
	return res;
}

std::ostream& operator<<(std::ostream& out, const FSNode& node)
{
	for (size_t i = 0; i < node.GetLevel(); i++)
		out << '.';
	out << (node.IsDir() ? "D" : "F") << " " << node.GetName() << std::endl;
	return out;
}
