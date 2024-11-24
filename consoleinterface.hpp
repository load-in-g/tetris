#ifndef _CONSOLE_INTERAFACE
#define _CONSOLE_INTERAFACE

#include <windows.h>
#include "gui.hpp"

class ConsoleInterface : public GUI {
public:
	ConsoleInterface();
	~ConsoleInterface();

	void SetWindow(int Width, int Height);

	void DrawBorders();

	void SetStart();
	void SetRestart();

	void AddSurfaceToScreen(Surface& surface, int start_x, int start_y);
	void Clear();
	void Statistics(int left_up_x, int left_up_y, Shape& next_shape, Shape& backpack_shape, GlobalStatistic& statistic);

	void Draw();

private:
	wchar_t game_over_text[11] = L"GAME OVER.";
	wchar_t restart_game_text[11] = L" RESTART. ";
	wchar_t enter_text[11] = L" (ENTER)  ";
	wchar_t start_game_text[11] = L"START GAME";

	HANDLE _hConsole;

	wchar_t* _screen;
};

#endif