#include "consoleinterface.hpp"

#include <windows.h>
#include <cmath>
#include <iostream>

#pragma comment (lib, "User32.lib")

ConsoleInterface::ConsoleInterface() {
	this->_screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT + 1];

	this->SetWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
}

ConsoleInterface::~ConsoleInterface() {
	delete[] this->_screen;
}

void ConsoleInterface::DrawBorders() {
	for (int x = START_X; x < START_X + SURFACE_WIDTH; x++) {
		this->_screen[0 * SCREEN_WIDTH + x] = 0x2550;
		this->_screen[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + x] = 0x2550;
	}
	for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
		this->_screen[y * SCREEN_WIDTH + START_X - 1] = 0x2551;
		this->_screen[y * SCREEN_WIDTH + START_X + SURFACE_WIDTH] = 0x2551;
	}
	this->_screen[0 * SCREEN_WIDTH + START_X - 1] = 0x2554;
	this->_screen[START_X + SURFACE_WIDTH] = 0x2557;
	this->_screen[(SURFACE_HEIGHT + 1) * SCREEN_WIDTH + START_X - 1] = 0x255A;
	this->_screen[(SURFACE_HEIGHT + 1) * SCREEN_WIDTH + START_X + SURFACE_WIDTH] = 0x255D;
}

void ConsoleInterface::Clear() {
	wmemset(this->_screen, NO_SYMBOL, (SCREEN_WIDTH * SCREEN_HEIGHT + 1));
	this->_screen[SCREEN_WIDTH * SCREEN_HEIGHT] = 0;
}

void ConsoleInterface::SetStart() {
	memcpy(&(this->_screen[11 * SCREEN_WIDTH + 1]), this->start_game_text, SURFACE_WIDTH * sizeof(wchar_t));
	memcpy(&(this->_screen[12 * SCREEN_WIDTH + 1]), this->enter_text, SURFACE_WIDTH * sizeof(wchar_t));
}

void ConsoleInterface::SetRestart() {
	memcpy(&(this->_screen[9 * SCREEN_WIDTH + 1]), this->game_over_text, SURFACE_WIDTH * sizeof(wchar_t));
	memcpy(&(this->_screen[11 * SCREEN_WIDTH + 1]), this->restart_game_text, SURFACE_WIDTH * sizeof(wchar_t));
	memcpy(&(this->_screen[12 * SCREEN_WIDTH + 1]), this->enter_text, SURFACE_WIDTH * sizeof(wchar_t));
}

void ConsoleInterface::Statistics(int left_up_x, int left_up_y, Shape& next_shape, Shape& backpack_shape, GlobalStatistic& statistic) {
	wchar_t time_text[STATISTICS_LINE_LENGTH] = L"Time: XX:XX    ";
	wchar_t next_shape_text[STATISTICS_LINE_LENGTH] = L"Backpack shape:";
	wchar_t backpack_shape_text[STATISTICS_LINE_LENGTH] = L"Next shape:    ";
	wchar_t rows_text[STATISTICS_LINE_LENGTH] = L"Rows: XXXXXXXX ";
	wchar_t points_text[STATISTICS_LINE_LENGTH] = L"Points: XXXXXX ";
	wchar_t difficulty_text[STATISTICS_LINE_LENGTH] = L"Difficulty: XX ";

	statistic.current_time = time(NULL);
	statistic.elapsed_time = statistic.current_time - statistic.game_start_time;

	time_text[6] = (statistic.elapsed_time) / 60 % 60 / 10 + L'0';
	time_text[7] = (statistic.elapsed_time) / 60 % 60 % 10 + L'0';
	time_text[9] = (statistic.elapsed_time) % 60 / 10 + L'0';
	time_text[10] = (statistic.elapsed_time) % 60 % 10 + L'0';

	int flag = 0;
	for (int i = 6; i < 14; i++) {
		int res = statistic.rows % (int)pow((int)10, (int)(14 - i)) / (int)pow((int)10, (int)(13 - i));

		if (flag || res || i == 13)
			rows_text[i] = res + L'0';
		else
			rows_text[i] = NO_SYMBOL;

		if (res) flag = 1;
	}

	flag = 0;
	for (int i = 8; i < 14; i++) {
		int res = statistic.points % (int)pow((int)10, (int)(14 - i)) / (int)pow((int)10, (int)(13 - i));

		if (flag || res || i == 13)
			points_text[i] = res + L'0';
		else
			points_text[i] = NO_SYMBOL;

		if (res) flag = 1;
	}

	flag = 0;
	for (int i = 12; i < 14; i++) {
		int res = statistic.difficulty % (int)pow((int)10, (int)(14 - i)) / (int)pow((int)10, (int)(13 - i));

		if (flag || res || i == 13)
			difficulty_text[i] = res + L'0';
		else
			difficulty_text[i] = NO_SYMBOL;

		if (res) flag = 1;
	}


	memcpy(&(this->_screen[(left_up_y + 1) * SCREEN_WIDTH + (left_up_x + 1)]), time_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));

	memcpy(&(this->_screen[(left_up_y + 3) * SCREEN_WIDTH + (left_up_x + 1)]), next_shape_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			this->_screen[(left_up_y + 4 + y) * SCREEN_WIDTH + (left_up_x + 6 + x)] = (next_shape.GetArray(y, x)) ? MAIN_SYMBOL : NO_SYMBOL;
		}
	}

	memcpy(&(this->_screen[(left_up_y + 9) * SCREEN_WIDTH + (left_up_x + 1)]), backpack_shape_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			this->_screen[(left_up_y + 10 + y) * SCREEN_WIDTH + (left_up_x + 6 + x)] = (backpack_shape.GetArray(y, x)) ? MAIN_SYMBOL : NO_SYMBOL;
		}
	}

	memcpy(&(this->_screen[(left_up_y + 15) * SCREEN_WIDTH + (left_up_x + 1)]), rows_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	memcpy(&(this->_screen[(left_up_y + 17) * SCREEN_WIDTH + (left_up_x + 1)]), points_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	memcpy(&(this->_screen[(left_up_y + 19) * SCREEN_WIDTH + (left_up_x + 1)]), difficulty_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));

	return;
}


void ConsoleInterface::AddSurfaceToScreen(Surface& surface, int start_x, int start_y) {
	for (int x = 0; x < SURFACE_WIDTH; x++) {
		for (int y = 0; y < SURFACE_HEIGHT; y++) {
			this->_screen[(start_y + y) * SCREEN_WIDTH + (start_x + x)] = surface.GetSurface(x, y);
		}
	}
}

void ConsoleInterface::SetWindow(int Width, int Height) {
	system("title Tetris");
	system("color 0b");

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX font = {};
	GetCurrentConsoleFontEx(hConsole, false, &font);
	font.dwFontSize = { 0, 29 };
	font.cbSize = sizeof(font);
	font.FontWeight = 1000;
	lstrcpyW(font.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(hConsole, false, &font);

	HWND window = FindWindowW(NULL, L"Tetris");
	SetWindowPos(window, HWND_TOP, -7, -1, 0, 0, SWP_NOSIZE);

	_COORD coord;
	coord.X = Width;
	coord.Y = Height;
	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(Handle, coord);
	SetConsoleWindowInfo(Handle, TRUE, &Rect);

	this->_hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(this->_hConsole);
}

void ConsoleInterface::Draw() {
	DWORD dwBytesWritten = 0;
	WriteConsoleOutputCharacterW(this->_hConsole, this->_screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &dwBytesWritten);
}