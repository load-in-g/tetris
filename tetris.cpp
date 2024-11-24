#include "tetris.hpp"
#include <windows.h>

Tetris::Tetris() {
	nullshape.NullShape();
}

Tetris::~Tetris() {
	return;
}

void Tetris::ResetStatistic() {
	this->statistic.game_start_time = time(NULL);
	this->statistic.current_time = -1;
	this->statistic.elapsed_time = 0;

	this->statistic.rows = 0;
	this->statistic.difficulty = 1;
	this->statistic.points = 0;

	this->statistic.can_use_backpack = 1;

	this->statistic.run = 1;
}

void Tetris::DownFunc(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic) {
	std::unique_lock<std::mutex> lock(*mut);

	*down_delay = BASE_DOWN_DELAY - statistic->difficulty * CONVERT_DIFFICULTY_TO_DELAY;

	while (statistic->run) {
		if (*down_delay == HARD_DROP_DELAY) {
			statistic->points += 2 * statistic->difficulty;
		}

		if (*down_delay == SOFT_DROP_DELAY) {
			*down_delay = BASE_DOWN_DELAY - statistic->difficulty * CONVERT_DIFFICULTY_TO_DELAY;
			statistic->points++;
		}

		condition->wait_for(lock, std::chrono::milliseconds(*down_delay));
		actions->MoveDown = 1;
	}
}

void Tetris::MainCycle(GUI& screen) {
	while (this->statistic.run) {
		screen.Statistics(SURFACE_WIDTH + 3, 0, next_shape, backpack_shape, this->statistic);

		current_shape.MoveShape(actions, backpack_shape, surface);

		surface_with_shape_and_shadow = surface;

		current_shape.AddShadowToSurface(surface_with_shape_and_shadow);

		if (current_shape.AddShapeToSurface(surface_with_shape_and_shadow, MAIN_SYMBOL)) {
			down_delay = BASE_DOWN_DELAY - this->statistic.difficulty * CONVERT_DIFFICULTY_TO_DELAY;
			current_shape.MoveUp();
			current_shape.AddShapeToSurface(surface, MAIN_SYMBOL);

			current_shape = nullshape;

			surface.RemoveLines(this->statistic);
			Sleep(BASE_STREAM_MOVE_DELAY);
			surface.DropLines();

			this->statistic.can_use_backpack = 1;
		}

		if (current_shape == nullshape) {
			current_shape = next_shape;
			next_shape = generator.RemoveShapeFromList();

			if (current_shape.AddShapeToSurface(surface, NO_SYMBOL))
				this->statistic.run = 0;
		}

		screen.AddSurfaceToScreen(surface_with_shape_and_shadow, START_X, START_Y);

		screen.Draw();

		Sleep(DEFAULT_DELAY_IN_WHILE_CYCLE);
	}
}

void Tetris::Run(GUI& screen, Control& controller) {
	screen.Clear();
	screen.DrawBorders();
	screen.SetStart();
	screen.Draw();

	controller.WaitAction();

	while (1) {
		this->surface.Clear();
		this->surface_with_shape_and_shadow.Clear();
		screen.Clear();
		screen.DrawBorders();

		this->down_delay = BASE_DOWN_DELAY - this->statistic.difficulty * CONVERT_DIFFICULTY_TO_DELAY;

		this->ResetStatistic();

		this->backpack_shape = this->nullshape;
		this->current_shape = this->generator.RemoveShapeFromList();
		this->next_shape = this->generator.RemoveShapeFromList();


		controller.TrackActions(&this->down_delay, &mut, &condition, &actions, &statistic);
		std::thread th2(&Tetris::DownFunc, this, &this->down_delay, &mut, &condition, &actions, &statistic);

		this->MainCycle(screen);

		th2.join();


		screen.AddSurfaceToScreen(this->surface, START_X, START_Y);
		screen.SetRestart();
		screen.Draw();

		controller.WaitAction();
	}
}