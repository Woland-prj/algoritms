#include "TUI.h"
#include <algorithm>

TUI::TUI(FileSystem& fs)
	: m_fs(fs)
{
	m_fs.ResetToRoot();
	const auto& root = m_fs.GetCurrent();
	m_path = { root.GetName() };

	setlocale(LC_ALL, "ru_RU.UTF-8");
	initscr();
	noecho();
	raw();
	keypad(stdscr, TRUE);
	curs_set(0);

	start_color();
	use_default_colors();
	init_pair(1, COLOR_CYAN, -1); // директории
	init_pair(2, COLOR_WHITE, -1); // файлы
	init_pair(3, COLOR_BLACK, COLOR_WHITE); // выделение
	init_pair(4, COLOR_GREEN, -1); // статус
	init_pair(5, COLOR_RED, -1); // статус
	init_pair(6, COLOR_MAGENTA, -1); // статус

	int sy, sx;
	getmaxyx(stdscr, sy, sx);
	m_rootWin = newwin(sy, sx, 0, 0);
	keypad(m_rootWin, TRUE);
	nodelay(m_rootWin, FALSE);
	refresh();
	wrefresh(m_rootWin);
}

TUI::~TUI()
{
	wclear(m_rootWin);
	wrefresh(m_rootWin);
	delwin(m_rootWin);
	endwin();
}

void TUI::Run()
{
	bool running = true;
	Draw();

	while (running)
	{
		int ch = wgetch(m_rootWin);
		if (ch == K_QUIET)
		{
			running = false;
		}
		else
		{
			HandleInput(ch);
			Draw();
		}
	}
}

void TUI::HandleInput(int ch)
{
	switch (ch)
	{
	case KEY_UP:
	case K_VIM_UP:
		MoveSelection(-1);
		break;
	case KEY_DOWN:
	case K_VIM_DOWN:
		MoveSelection(1);
		break;
	case KEY_RIGHT:
	case KEY_ENTER:
	case K_MAC_ENTER:
		OpenSelected();
		break;
	case KEY_LEFT:
	case KEY_BACKSPACE:
	case K_CH_BACKSPACE:
		GoBack();
		break;
	case K_VIM_COPY:
		DoCopy();
		break;
	case K_VIM_CUT:
		DoMove();
		break;
	case K_VIM_PASTE:
		DoPaste();
		break;
	case K_VIM_RENAME:
		DoRename();
		break;
	case K_VIM_DELETE:
		DoDelete();
		break;
	case K_VIM_ADD:
		DoCreate();
		break;
	case K_HELP:
		ShowHelpDialog();
		break;
	default:
		break;
	}
}

void TUI::MoveSelection(int delta)
{
	const auto& inode = m_fs.GetCurrent();
	if (inode.GetChildren().empty())
		return;
	m_selected = std::clamp(m_selected + delta, 0, static_cast<int>((inode.GetChildren().size() - 1)));
}

void TUI::OpenSelected()
{
	const auto& inode = m_fs.GetCurrent();
	if (inode.GetChildren().empty())
		return;

	try
	{
		m_fs.Open(m_selected);
		m_path.push_back(inode.GetChildren()[m_selected]->GetName());
		m_selected = 0;
	}
	catch (const std::exception& ex)
	{
		DrawStatus(ex.what());
	}
}

void TUI::GoBack()
{
	if (m_path.size() <= 1)
		return;
	m_fs.MoveUp();
	m_path.pop_back();
	m_selected = 0;
}

void TUI::Draw()
{
	werase(m_rootWin);
	DrawHeaderBox(" Woland commander ");
	DrawPath();
	DrawFileList();
	DrawMode();
	DrawHelp();
	wrefresh(m_rootWin);
}

void TUI::DrawHeaderBox(const std::string& title)
{
	box(m_rootWin, 0, 0);
	mvwprintw(m_rootWin, 0, 2, "%s", title.c_str());
}

void TUI::DrawPath()
{
	std::string path;
	for (size_t i = 0; i < m_path.size(); i++)
		path += (i == 0 ? "" : "/") + m_path[i];
	mvwprintw(m_rootWin, 1, 2, "%s", path.c_str());
	mvwhline(m_rootWin, 2, 1, ACS_HLINE, getmaxx(m_rootWin) - 2);
}

void TUI::DrawFileList()
{
	const auto& inode = m_fs.GetCurrent();
	int y = 4;
	const auto& children = inode.GetChildren();

	for (size_t i = 0; i < children.size(); ++i)
	{
		const auto& name = children[i]->GetName();
		bool isSelected = static_cast<int>(i) == m_selected;
		bool isDir = children[i]->IsDir();

		if (isSelected)
			wattron(m_rootWin, COLOR_PAIR(3) | A_BOLD);
		else if (isDir)
			wattron(m_rootWin, COLOR_PAIR(1));
		else
			wattron(m_rootWin, COLOR_PAIR(2));

		mvwprintw(m_rootWin, y++, 4, "%s%s", name.c_str(), isDir ? "/" : "");

		wattroff(m_rootWin, COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BOLD);
	}

	if (inode.GetChildren().empty())
		mvwprintw(m_rootWin, y, 4, "(пусто)");
}

void TUI::DrawStatus(const std::string& msg)
{
	int sy, sx;
	getmaxyx(m_rootWin, sy, sx);
	wattron(m_rootWin, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(m_rootWin, sy - 2, 2, "%s", msg.c_str());
	wattroff(m_rootWin, COLOR_PAIR(4) | A_BOLD);
	wrefresh(m_rootWin);
}

void TUI::DrawHelp()
{
	int sy, sx;
	getmaxyx(m_rootWin, sy, sx);
	mvwprintw(m_rootWin, 1, sx - HELP_MSG_SIZE - 1, HELP_MSG);
	wrefresh(m_rootWin);
}

void TUI::DrawMode()
{
	std::string msg = "NORMAL";
	int pairId = 4;
	if (m_fs.IsMoving())
	{
		msg = "CUT";
		pairId = 5;
	}
	else if (m_fs.IsCopeing())
	{
		msg = "COPY";
		pairId = 6;
	}
	wattron(m_rootWin, COLOR_PAIR(pairId) | A_BOLD);
	int sy, sx;
	getmaxyx(m_rootWin, sy, sx);
	mvwprintw(m_rootWin, sy - 2, 2, "%s", msg.c_str());
	wattroff(m_rootWin, COLOR_PAIR(pairId) | A_BOLD);
	wrefresh(m_rootWin);
}

void TUI::DoCopy()
{
	if (m_fs.IsMoving() || m_fs.IsCopeing())
		return;
	m_fs.Copy(m_selected);
}

void TUI::DoMove()
{
	if (m_fs.IsMoving() || m_fs.IsCopeing())
		return;
	m_fs.Cut(m_selected);
};

void TUI::DoPaste()
{
	if (!m_fs.IsMoving() && !m_fs.IsCopeing())
		return;
	m_fs.Paste();
};

std::string TUI::ShowRenameDialog(const std::string& oldName)
{
	int sy, sx;
	getmaxyx(stdscr, sy, sx);

	int winH = 5;
	int winW = 40;
	int winY = sy / 2 - winH / 2;
	int winX = sx / 2 - winW / 2;

	WINDOW* win = newwin(winH, winW, winY, winX);
	box(win, 0, 0);
	mvwprintw(win, 0, 2, " Переименовать ");
	mvwprintw(win, 1, 2, "Старое имя: %s", oldName.c_str());
	mvwprintw(win, 2, 2, "Новое имя: ");

	std::string newName = oldName;
	int cursorPos = static_cast<int>(newName.size());
	wrefresh(win);

	echo();
	curs_set(1);

	WINDOW* inputWin = derwin(win, 1, winW - 14, 2, 13);
	if (!inputWin)
	{
		noecho();
		curs_set(0);
		delwin(win);
		return oldName;
	}

	waddstr(inputWin, newName.c_str());
	wmove(inputWin, 0, cursorPos);
	wrefresh(inputWin);

	int ch;
	while ((ch = wgetch(inputWin)) != '\n')
	{
		if (ch == K_MAC_ESC)
		{
			newName = oldName;
			break;
		}
		else if (ch == KEY_BACKSPACE || ch == K_MAC_BACKSPACE || ch == K_CH_BACKSPACE)
		{
			if (!newName.empty() && cursorPos > 0)
			{
				newName.erase(cursorPos - 1, 1);
				cursorPos--;
				werase(inputWin);
				waddstr(inputWin, newName.c_str());
				wmove(inputWin, 0, cursorPos);
			}
		}
		else if (ch >= K_SPACE)
		{ // printable ASCII
			newName.insert(cursorPos, 1, static_cast<char>(ch));
			cursorPos++;
			werase(inputWin);
			waddstr(inputWin, newName.c_str());
			wmove(inputWin, 0, cursorPos);
		}
		wrefresh(inputWin);
	}

	delwin(inputWin);
	noecho();
	curs_set(0);
	delwin(win);
	redrawwin(m_rootWin);
	wrefresh(m_rootWin);

	return newName;
}

bool TUI::ShowDeleteConfirmDialog(const std::string& name)
{
	int sy, sx;
	getmaxyx(stdscr, sy, sx);

	int win_h = 5;
	int win_w = 40;
	int win_y = sy / 2 - win_h / 2;
	int win_x = sx / 2 - win_w / 2;

	WINDOW* win = newwin(win_h, win_w, win_y, win_x);
	box(win, 0, 0);
	mvwprintw(win, 0, 2, " Подтверждение ");
	mvwprintw(win, 1, 2, "Удалить \"%s\"?", name.c_str());
	mvwprintw(win, 2, 2, "(y/n): ");
	wrefresh(win);

	int ch;
	do
	{
		ch = wgetch(win);
		if (ch == 'y' || ch == 'Y')
		{
			delwin(win);
			return true;
		}
		if (ch == 'n' || ch == 'N' || ch == 27)
		{
			delwin(win);
			return false;
		}
	} while (true);
}

void TUI::DoRename()
{
	const auto& inode = m_fs.GetCurrent();
	if (inode.GetChildren().empty() || m_selected < 0)
		return;

	const auto* node = inode.GetChildren()[m_selected];
	std::string oldName = node->GetName();
	std::string newName = ShowRenameDialog(oldName);

	if (newName != oldName && !newName.empty())
	{
		try
		{
			m_fs.Rename(m_selected, newName);
		}
		catch (const std::exception& ex)
		{
			DrawStatus(ex.what());
		}
	}
}

void TUI::DoDelete()
{
	const auto& inode = m_fs.GetCurrent();
	if (inode.GetChildren().empty() || m_selected < 0)
		return;

	const auto* node = inode.GetChildren()[m_selected];
	std::string name = node->GetName();

	if (ShowDeleteConfirmDialog(name))
	{
		try
		{
			m_fs.Delete(m_selected);
			if (m_selected >= static_cast<int>(inode.GetChildren().size()))
			{
				m_selected = static_cast<int>(inode.GetChildren().size()) - 1;
			}
			if (m_selected < 0)
				m_selected = 0;
		}
		catch (const std::exception& ex)
		{
			DrawStatus(ex.what());
		}
	}
}

std::string TUI::ShowCreateDialog()
{
	int sy, sx;
	getmaxyx(stdscr, sy, sx);

	int winH = 5;
	int winW = 40;
	int winY = sy / 2 - winH / 2;
	int winX = sx / 2 - winW / 2;

	WINDOW* win = newwin(winH, winW, winY, winX);
	box(win, 0, 0);
	mvwprintw(win, 0, 2, " Создать файл/папку ");
	mvwprintw(win, 1, 2, "Имя (с / в конце — папка):");
	mvwprintw(win, 2, 2, "> ");
	wrefresh(win);

	echo();
	curs_set(1);

	WINDOW* inputWin = derwin(win, 1, winW - 4, 2, 4);
	if (!inputWin)
	{
		noecho();
		curs_set(0);
		delwin(win);
		return "";
	}

	std::string name;
	int ch;
	werase(inputWin);
	wrefresh(inputWin);
	while ((ch = wgetch(inputWin)) != '\n')
	{
		if (ch == K_MAC_ESC)
		{
			name.clear();
			break;
		}
		else if (ch == KEY_BACKSPACE || ch == K_MAC_BACKSPACE || ch == K_CH_BACKSPACE)
		{
			if (!name.empty())
			{
				name.pop_back();
				werase(inputWin);
				waddstr(inputWin, name.c_str());
			}
		}
		else if (ch >= K_SPACE)
		{
			name += static_cast<char>(ch);
			waddstr(inputWin, name.c_str() + (name.size() - 1));
		}
		werase(inputWin);
		waddstr(inputWin, name.c_str());
		wmove(inputWin, 0, static_cast<int>(name.size()));
		wrefresh(inputWin);
	}

	delwin(inputWin);
	noecho();
	curs_set(0);
	delwin(win);
	redrawwin(m_rootWin);
	wrefresh(m_rootWin);

	return name;
}

void TUI::ShowHelpDialog()
{
	int sy, sx;
	getmaxyx(stdscr, sy, sx);

	int winH = HELP_WIN_MSG_LEN + 2;
	int winW = 50;
	int winY = sy / 2 - winH / 2;
	int winX = sx / 2 - winW / 2;

	WINDOW* win = newwin(winH, winW, winY, winX);
	box(win, 0, 0);
	mvwprintw(win, 0, 2, " Справка ");
	for (size_t i = 0; i < HELP_WIN_MSG_LEN; i++)
	{
		mvwprintw(win, i + 1, 2, HELP_WIN_MSG[i]);
	}
	wrefresh(win);

	int ch = 0;
	while (ch != K_QUIET && ch != K_MAC_ESC)
		ch = wgetch(win);

	delwin(win);
	redrawwin(m_rootWin);
	wrefresh(m_rootWin);
}

void TUI::DoCreate()
{
	std::string input = ShowCreateDialog();
	if (input.empty())
		return;

	bool isDir = false;
	std::string name = input;

	if (!name.empty() && name.back() == '/')
	{
		isDir = true;
		name.pop_back();
	}

	if (name.empty())
	{
		DrawStatus("Имя не может быть пустым");
		return;
	}

	try
	{
		if (isDir)
		{
			m_fs.CreateFolder(name);
		}
		else
		{
			m_fs.CreateFile(name);
		}
		const auto& inode = m_fs.GetCurrent();
		const auto children = inode.GetChildren();
		for (size_t i = 0; i < children.size(); ++i)
		{
			if (children[i]->GetName() == name)
			{
				m_selected = static_cast<int>(i);
				break;
			}
		}
	}
	catch (const std::exception& ex)
	{
		DrawStatus(ex.what());
	}
}
