#include "TUI.h"
#include <algorithm>

TUI::TUI(FileSystem& fs)
	: m_fs(fs)
{
	m_fs.ResetToRoot();
	const auto& root = m_fs.GetCurrent();
	m_path = { root.GetName() };

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
		if (ch == 'q' || ch == 'Q')
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
	DrawHeaderBox(" Woland comander ");
	DrawPath();
	DrawFileList();
	DrawMode();
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
