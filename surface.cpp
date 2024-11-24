#include "surface.hpp"
#include <windows.h>
#include <iostream>

Surface::Surface() {
	this->_surface = new wchar_t* [SURFACE_WIDTH];

	for (int count = 0; count < SURFACE_WIDTH; count++)
		this->_surface[count] = new wchar_t[SURFACE_HEIGHT];
}

Surface::~Surface() {
	for (int count = 0; count < SURFACE_WIDTH; count++)
		delete[] this->_surface[count];

	delete[] this->_surface;
}

Surface& Surface::operator=(const Surface& other) {
	for (int x = 0; x < SURFACE_WIDTH; x++)
		wmemcpy(this->_surface[x], other._surface[x], SURFACE_HEIGHT);

	return *this;
}

void Surface::Clear() {
	for (int x = 0; x < SURFACE_WIDTH; x++)
		wmemset(this->_surface[x], NO_SYMBOL, SURFACE_HEIGHT);
}

void Surface::RemoveLines(GlobalStatistic& statistic) {
	int lines_dropped = 0;
	for (int y = 0; y < SURFACE_HEIGHT; y++) {
		int count = 0;
		for (int x = 0; x < SURFACE_WIDTH; x++) {
			if (this->_surface[x][y] == MAIN_SYMBOL)
				count++;
		}

		if (count == SURFACE_WIDTH) {
			lines_dropped++;
			for (int x = 0; x < SURFACE_WIDTH; x++) {
				this->_surface[x][y] = ' ';
			}
		}
	}

	if (lines_dropped) {
		statistic.rows += lines_dropped;

		switch (lines_dropped) {
		case 1:
			statistic.points += 100 * statistic.difficulty;
			break;
		case 2:
			statistic.points += 300 * statistic.difficulty;
			break;
		case 3:
			statistic.points += 500 * statistic.difficulty;
			break;
		case 4:
			statistic.points += 800 * statistic.difficulty;
			break;
		}

		if (statistic.rows % 10 == 0 && statistic.difficulty < 30)
			statistic.difficulty++;
	}
}

void Surface::DropLines() {
	for (int y = 0; y < SURFACE_HEIGHT; y++) {
		int count = 0;
		for (int x = 0; x < SURFACE_WIDTH; x++) {
			if (this->_surface[x][y] != MAIN_SYMBOL)
				count++;
		}

		if (count == SURFACE_WIDTH) {
			for (int drop_y = y - 1; drop_y >= 0; drop_y--) {
				for (int drop_x = 0; drop_x < SURFACE_WIDTH; drop_x++) {
					this->_surface[drop_x][drop_y + 1] = this->_surface[drop_x][drop_y];
					this->_surface[drop_x][drop_y] = NO_SYMBOL;
				}
			}
		}
	}
}

wchar_t Surface::GetSurface(int x, int y) {
	return this->_surface[x][y];
}
void Surface::SetSurface(int x, int y, wchar_t sym) {
	this->_surface[x][y] = sym;
}