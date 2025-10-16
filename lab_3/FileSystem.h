#ifndef TREE_H
#define TREE_H
#include "FSNode.h"
#include <vector>

class FileSystem final
{
public:
	FileSystem() = delete;
	explicit FileSystem(const std::string& rootName);
	explicit FileSystem(std::istream& in);

	~FileSystem() = default;

	[[nodiscard]] FSNode& GetCurrent() const;
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
	[[nodiscard]] bool IsCopeing() const;
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
