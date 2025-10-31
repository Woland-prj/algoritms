// библиотека отрисовки интерфейса UNIX ncurses:
// https://en.wikipedia.org/wiki/Ncurses
// https://github.com/mcdaniel/curses_tutorial
#ifndef TUI_H
#define TUI_H
#include "FileSystem.h"
#include <ncurses.h>
#include <string>
#include <vector>

static constexpr int K_MAC_ENTER = 10;
static constexpr int K_MAC_ESC = 27;
static constexpr int K_MAC_BACKSPACE = 127;
static constexpr int K_VIM_UP = 'k';
static constexpr int K_VIM_DOWN = 'j';
static constexpr int K_CH_BACKSPACE = '\b';
static constexpr int K_VIM_CUT = 'x';
static constexpr int K_VIM_COPY = 'y';
static constexpr int K_VIM_PASTE = 'p';
static constexpr int K_VIM_RENAME = 'r';
static constexpr int K_VIM_DELETE = 'd';
static constexpr int K_HELP = 'h';
static constexpr int K_SPACE = 32;
static constexpr int K_TILD = 126;
static constexpr int K_VIM_ADD = 'a';
static constexpr int K_QUIET = 'q';

static constexpr auto HELP_MSG = " q - выход | h - справка ";
static constexpr auto HELP_MSG_SIZE = 25;

static constexpr const char* HELP_WIN_MSG[] = {
	"q / Esc - выход",
	"← - вверх к родительской директории",
	"→ / Enter - открыть директорию",
	"↑ / k - передвинуть курсор вверх",
	"↓ / j - передвинуть курсор вниз",
	"a - новый файл / директория",
	"y - скопировать файл / директория",
	"x - вырезать файл / директория",
	"p - вставить файл / директорию",
	"r - переименовать файл / директорию",
	"d - удалить файл / директорию",
};

static constexpr int HELP_WIN_MSG_LEN = 11;

class TUI
{
public:
	explicit TUI(FileSystem& fs);
	void Run();
	~TUI();

private:
	FileSystem& m_fs;
	WINDOW* m_rootWin{};
	int m_selected = 0;
	std::vector<std::string> m_path;

	void Draw();
	void DrawHeaderBox(const std::string& title);
	void DrawPath();
	void DrawFileList();
	void DrawStatus(const std::string& msg);
	void DrawHelp();
	void DrawMode();

	void HandleInput(int ch);
	void MoveSelection(int delta);
	void OpenSelected();
	void GoBack();
	void DoCopy();
	void DoMove();
	void DoPaste();
	std::string ShowRenameDialog(const std::string& oldName);
	bool ShowDeleteConfirmDialog(const std::string& name);
	std::string ShowCreateDialog();
	void ShowHelpDialog();
	void DoCreate();
	void DoRename();
	void DoDelete();
};
#endif // TUI_H
