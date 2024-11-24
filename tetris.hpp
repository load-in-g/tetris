#ifndef _TETRIS
#define _TETRIS

#include "gui.hpp"
#include "control.hpp"

#include "consts_and_structs.hpp"
#include "generator.hpp"
#include <windows.h>

class Tetris {
public:
	Tetris();
	~Tetris();
	void ResetStatistic();

	void DownFunc(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic);

	void MainCycle(GUI& screen);
	void Run(GUI& screen, Control& controller);

private:
	std::mutex mut;
	std::condition_variable condition;

	Surface surface;
	Surface surface_with_shape_and_shadow;

	Actions actions;
	GlobalStatistic statistic;
	Generator generator;

	Shape nullshape;
	Shape current_shape;
	Shape next_shape;
	Shape backpack_shape;

	int down_delay;
};

#endif