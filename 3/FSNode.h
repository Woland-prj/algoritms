#ifndef FSNODE_H
#define FSNODE_H
#include <vector>

class FSNode final
{
	friend class FileSystem;

public:
	FSNode(std::string name, const bool isDir, const int16_t level, FSNode* parent);
	FSNode(const FSNode& other);
	std::string GetName() const;
	bool IsDir() const;
	int16_t GetLevel() const;
	std::vector<FSNode*> GetChildren() const;

private:
	std::string m_name;
	bool m_isDir;
	FSNode* m_parent;
	mutable int16_t m_level;
	std::vector<std::unique_ptr<FSNode>> m_children;
};

std::ostream& operator<<(std::ostream& out, const FSNode& node);

#endif // FSNODE_H
