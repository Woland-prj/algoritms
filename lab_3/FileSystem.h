#ifndef TREE_H
#define TREE_H
#include <vector>

struct FSNode
{
	std::string m_name;
	bool m_isDir;
	FSNode* m_parent;
	mutable int16_t m_level;
	std::vector<std::unique_ptr<FSNode>> m_children;
	FSNode(std::string name, const bool isDir, const int16_t level, FSNode* parent)
		: m_name(std::move(name))
		, m_isDir(isDir)
		, m_parent(parent)
		, m_level(level) {};
	FSNode(const FSNode& other)
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
};

struct Inode
{
	const std::string& name{};
	bool isDir = false;
	std::vector<std::string> children{};

	explicit Inode(const FSNode& fsNode)
	: name(fsNode.m_name)
	, isDir(fsNode.m_isDir)
	{
		for (const std::unique_ptr<FSNode>& child: fsNode.m_children)
		{
			children.push_back(child->m_name);
		}
	}
};

class FileSystem final
{
public:
	FileSystem() = delete;
	explicit FileSystem(const std::string& rootName);
	explicit FileSystem(std::istream& in);

	~FileSystem() = default;

	[[nodiscard]] Inode GetCurrent() const;
	void ResetToRoot();
	bool MoveUp();
	void Open(uint16_t childIndex);
	void CreateFolder(const std::string& folderName);
	void CreateFile(const std::string& fileName);
	void Copy(uint16_t childIndex);
	void Cut(uint16_t childIndex);
	void Rename(uint16_t childIndex, const std::string& newName) const;
	void Paste();
	void Delete(uint16_t childIndex) const;
	void Dump(std::ostream& out) const;
	[[nodiscard]] bool IsCoping() const;
	[[nodiscard]] bool IsMoving() const;

private:
	enum class FSMode
	{
		NORMAL,
		COPY,
		MOVE
	};
	std::unique_ptr<FSNode> m_root = nullptr;
	FSNode* m_current = nullptr;
	std::unique_ptr<FSNode> m_interactNode = nullptr;
	FSMode m_mode = FSMode::NORMAL;

	void CheckIndex(uint16_t childIndex) const;
	void ParseFile(std::istream& in);
	int16_t ParseLine(const std::string& strNode, int16_t prevLevel);
	static void RecalculateLevel(const FSNode& ptr);
	static void Out(std::ostream& out, const FSNode& ptr);
};

std::ostream& operator<<(std::ostream& out, const FileSystem& fs);

#endif // TREE_H
