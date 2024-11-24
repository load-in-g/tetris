#ifndef _CONST_AND_STRUCTS
#define _CONST_AND_STRUCTS

#include <ctime>

enum COORDINATES_ETC {
	SCREEN_WIDTH = 31,
	SCREEN_HEIGHT = 22,

	SURFACE_WIDTH = 10,
	SURFACE_HEIGHT = 20,

	// start - (1; 1) in screen array; - (0; 0) in surface array;
	START_X = 1,
	START_Y = 1,

	BASE_SHAPE_X = SURFACE_WIDTH / 2,
	BASE_SHAPE_Y = 1,

	STATISTICS_LINE_LENGTH = 16,
};

enum DELAYS {
	BASE_DOWN_DELAY = 1000,
	BASE_STREAM_MOVE_DELAY = 10,
	DEFAULT_DELAY_IN_WHILE_CYCLE = 10,

	SOFT_DROP_DELAY = 2,
	HARD_DROP_DELAY = 1,

	CONVERT_DIFFICULTY_TO_DELAY = 33,
};

enum SYMBOLS {
	NO_SYMBOL = 0x0020,
	MAIN_SYMBOL = 0x2593,
	SHADOWS_SYMBOL = 0x2592
};

struct GlobalStatistic {
	time_t game_start_time = -1;
	time_t current_time = -1;
	time_t elapsed_time = 0;

	int rows = 0;
	int difficulty = 1;
	int points = 0;

	int can_use_backpack = 1;

	int run = 1;
};

struct pixel {
	int x = -1;
	int y = -1;
};

struct Actions {
	int MoveUp = 0;
	int MoveDown = 0;
	int MoveLeft = 0;
	int MoveRight = 0;
	int RotateLeft = 0;
	int RotateRight = 0;

	int UseBackpack = 0;
};


#endif