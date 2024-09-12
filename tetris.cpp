#include <iostream>
#include <windows.h>
#include <ctime>
#include <chrono>
#include <thread>
#include <malloc.h>
#include <future>	
#include <cmath>

/*

The right center for every shape

Add combo bonus

Add T-spins(mini, regular, double)
*/

using namespace std;

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

	CONVERT_DIFFYCULTY_TO_DELAY = 33,
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
}GlobalStatistic;

class Shape {
public:
	struct Actions {
		int MoveUp = 0;
		int MoveDown = 0;
		int MoveLeft = 0;
		int MoveRight = 0;
		int RotateLeft = 0;
		int RotateRight = 0;

		int UseBackpack = 0;
	};

	Shape(int index = -1);
	~Shape();
	void rotate_right(wchar_t** surface);
	void rotate_left(wchar_t** surface);
	void generate();
	void MoveRight(wchar_t** surface);
	void MoveLeft(wchar_t** surface);
	void MoveUp();
	void MoveDown();
	void ArrayToCoords();
	void NullShape();
	int GetY();
	void MoveShape(Actions& Actions, Shape& backpack_shape, wchar_t** surface);
	void AddShadowToSurface(wchar_t** surface_with_shape_and_shadow);
	int AddShapeToSurface(wchar_t** surface, wchar_t sym);
	static void Statistics(wchar_t* screen, int left_up_x, int left_up_y, Shape& next_shape, Shape& backpack_shape);

	bool operator==(const Shape& other);

private:
	struct pixel{
		int x = -1;
		int y = -1;
	};

	pixel pixel_coordinates[4] = { -1 };
	int shape_array[4][4];
	int shape_index = -1;

	pixel position;

	int x_center = 1;
	int y_center = 1;

} nullshape;

class ShapesBasket {
public:
	ShapesBasket();
	~ShapesBasket();
	void AddShapeToList(int index);
	void GenerateShapeList();
	Shape RemoveShapeFromList();
	int IsEmpty();

private:
	int size = 0;
	struct ShapeList{
		Shape shape;
		ShapeList* p_next;
	};
	ShapeList* root;
};

void RemoveLines(wchar_t** surface);
void DropLines(wchar_t** surface);
void SetWindow(int Width, int Height);
void AddSurfaceToScreen(wchar_t* screen, wchar_t** surface, int start_x, int start_y);
void DrawBorders(wchar_t* screen);

void MoveFunc(int* down_delay, mutex* mut, condition_variable* condition, Shape::Actions* Actions);
void DownFunc(int* down_delay, wchar_t** surface, mutex* mut, condition_variable* condition, Shape::Actions* Actions);

int main(void) {
	nullshape.NullShape();

	srand(time(NULL));
	SetWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

	mutex mut;
	condition_variable condition;

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wchar_t game_over_text[] = L"GAME OVER.";
	wchar_t restart_game_text[] = L" RESTART. ";
	wchar_t enter_text[] = L" (ENTER)  ";
	wchar_t start_game_text[] = L"START GAME";

	wchar_t** surface = new wchar_t* [SURFACE_WIDTH];
	wchar_t** surface_with_shape_and_shadow = new wchar_t* [SURFACE_WIDTH];

	for (int count = 0; count < SURFACE_WIDTH; count++) {
		surface[count] = new wchar_t[SURFACE_HEIGHT];
		surface_with_shape_and_shadow[count] = new wchar_t[SURFACE_HEIGHT];
	}

	wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT + 1];
	wmemset(screen, NO_SYMBOL, (SCREEN_WIDTH * SCREEN_HEIGHT + 1));

	DrawBorders(screen);

	memcpy(&screen[11 * SCREEN_WIDTH + 1], start_game_text, SURFACE_WIDTH * sizeof(wchar_t));
	memcpy(&screen[12 * SCREEN_WIDTH + 1], enter_text, SURFACE_WIDTH * sizeof(wchar_t));

	WriteConsoleOutputCharacterW(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &dwBytesWritten);

	while (1) {
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
			break;
		Sleep(DEFAULT_DELAY_IN_WHILE_CYCLE);
	}

	while (1) {
		for (int x = 0; x < SURFACE_WIDTH; x++) {
			wmemset(surface[x], NO_SYMBOL, SURFACE_HEIGHT);
			wmemset(surface_with_shape_and_shadow[x], NO_SYMBOL, SURFACE_HEIGHT);
		}

		wmemset(screen, NO_SYMBOL, (SCREEN_WIDTH * SCREEN_HEIGHT + 1));
		DrawBorders(screen);

		screen[SCREEN_WIDTH * SCREEN_HEIGHT] = 0;

		int down_delay = BASE_DOWN_DELAY - GlobalStatistic.difficulty * CONVERT_DIFFYCULTY_TO_DELAY;

		GlobalStatistic.game_start_time = time(NULL);
		GlobalStatistic.current_time = -1;
		GlobalStatistic.elapsed_time = 0;

		GlobalStatistic.rows = 0;
		GlobalStatistic.difficulty = 1;
		GlobalStatistic.points = 0;

		GlobalStatistic.can_use_backpack = 1;

		GlobalStatistic.run = 1;

		ShapesBasket shapes_basket;

		shapes_basket.GenerateShapeList();

		Shape current_shape;
		Shape next_shape;
		Shape backpack_shape = nullshape;

		current_shape = shapes_basket.RemoveShapeFromList();
		next_shape = shapes_basket.RemoveShapeFromList();

		Shape::Actions Actions;

		thread th2(MoveFunc, &down_delay, &mut, &condition, &Actions);
		thread th1(DownFunc, &down_delay, surface, &mut, &condition, &Actions);

		while (GlobalStatistic.run) {
			Shape::Statistics(screen, SURFACE_WIDTH + 3, 0, next_shape, backpack_shape);

			current_shape.MoveShape(Actions, backpack_shape, surface);

			for (int x = 0; x < SURFACE_WIDTH; x++)
				wmemcpy(surface_with_shape_and_shadow[x], surface[x], SURFACE_HEIGHT);

			current_shape.AddShadowToSurface(surface_with_shape_and_shadow);

			if (current_shape.AddShapeToSurface(surface_with_shape_and_shadow, MAIN_SYMBOL)) {
				down_delay = BASE_DOWN_DELAY - GlobalStatistic.difficulty * CONVERT_DIFFYCULTY_TO_DELAY;
				current_shape.MoveUp();
				current_shape.AddShapeToSurface(surface, MAIN_SYMBOL);

				current_shape = nullshape;

				RemoveLines(surface);
				Sleep(BASE_STREAM_MOVE_DELAY);
				DropLines(surface);
				GlobalStatistic.can_use_backpack = 1;
			}

			if (current_shape == nullshape) {
				current_shape = next_shape;
				next_shape = shapes_basket.RemoveShapeFromList();

				if (shapes_basket.IsEmpty())
					shapes_basket.GenerateShapeList();

				if (current_shape.AddShapeToSurface(surface, NO_SYMBOL))
					GlobalStatistic.run = 0;
			}

			AddSurfaceToScreen(screen, surface_with_shape_and_shadow, START_X, START_Y);

			WriteConsoleOutputCharacterW(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &dwBytesWritten);

			Sleep(DEFAULT_DELAY_IN_WHILE_CYCLE);
		}

		th1.join();
		th2.join();

		AddSurfaceToScreen(screen, surface, START_X, START_Y);
		memcpy(&screen[9 * SCREEN_WIDTH + 1], game_over_text, SURFACE_WIDTH * sizeof(wchar_t));
		memcpy(&screen[11 * SCREEN_WIDTH + 1], restart_game_text, SURFACE_WIDTH * sizeof(wchar_t));
		memcpy(&screen[12 * SCREEN_WIDTH + 1], enter_text, SURFACE_WIDTH * sizeof(wchar_t));

		WriteConsoleOutputCharacterW(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &dwBytesWritten);

		while (1) {
			if (GetAsyncKeyState(VK_RETURN) & 0x0001)
				break;
			Sleep(DEFAULT_DELAY_IN_WHILE_CYCLE);
		}
	}

	for (int count = 0; count < SURFACE_WIDTH; count++) {
		delete[] surface[count];
		delete[] surface_with_shape_and_shadow[count];
	}

	delete[] surface;
	delete[] surface_with_shape_and_shadow;

	delete[] screen;
}

void SetWindow(int Width, int Height) {
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
}

void DrawBorders(wchar_t* screen) {
	for (int x = START_X; x < START_X + SURFACE_WIDTH; x++) {
		screen[0 * SCREEN_WIDTH + x] = 0x2550;
		screen[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + x] = 0x2550;
	}
	for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
		screen[y * SCREEN_WIDTH + START_X - 1] = 0x2551;
		screen[y * SCREEN_WIDTH + START_X + SURFACE_WIDTH] = 0x2551;
	}
	screen[0 * SCREEN_WIDTH + START_X - 1] = 0x2554;
	screen[START_X + SURFACE_WIDTH] = 0x2557;
	screen[(SURFACE_HEIGHT + 1) * SCREEN_WIDTH + START_X - 1] = 0x255A;
	screen[(SURFACE_HEIGHT + 1) * SCREEN_WIDTH + START_X + SURFACE_WIDTH] = 0x255D;
}

void AddSurfaceToScreen(wchar_t* screen, wchar_t** surface, int start_x, int start_y) {
	for (int x = 0; x < SURFACE_WIDTH; x++) {
		for (int y = 0; y < SURFACE_HEIGHT; y++) {
			screen[(start_y + y) * SCREEN_WIDTH + (start_x + x)] = surface[x][y];
		}
	}
}

void RemoveLines(wchar_t** surface) {
	int lines_dropped = 0;
	for (int y = 0; y < SURFACE_HEIGHT; y++) {
		int count = 0;
		for (int x = 0; x < SURFACE_WIDTH; x++) {
			if (surface[x][y] == MAIN_SYMBOL)
				count++;
		}

		if (count == SURFACE_WIDTH) {
			lines_dropped++;
			for (int x = 0; x < SURFACE_WIDTH; x++) {
				surface[x][y] = ' ';
			}
		}
	}

	if (lines_dropped) {
		GlobalStatistic.rows += lines_dropped;

		switch (lines_dropped) {
		case 1:
			GlobalStatistic.points += 100 * GlobalStatistic.difficulty;
			break;
		case 2:
			GlobalStatistic.points += 300 * GlobalStatistic.difficulty;
			break;
		case 3:
			GlobalStatistic.points += 500 * GlobalStatistic.difficulty;
			break;
		case 4:
			GlobalStatistic.points += 800 * GlobalStatistic.difficulty;
			break;
		}

		if (GlobalStatistic.rows % 10 == 0 && GlobalStatistic.difficulty < 30)
			GlobalStatistic.difficulty++;
	}
}

void DropLines(wchar_t** surface) {
	for (int y = 0; y < SURFACE_HEIGHT; y++) {
		int count = 0;
		for (int x = 0; x < SURFACE_WIDTH; x++) {
			if (surface[x][y] != MAIN_SYMBOL)
				count++;
		}

		if (count == SURFACE_WIDTH) {
			for (int drop_y = y - 1; drop_y >= 0; drop_y--) {
				for (int drop_x = 0; drop_x < SURFACE_WIDTH; drop_x++) {
					surface[drop_x][drop_y + 1] = surface[drop_x][drop_y];
					surface[drop_x][drop_y] = NO_SYMBOL;
				}
			}
		}
	}
}

void MoveFunc(int* down_delay, mutex* mut, condition_variable* condition, Shape::Actions* Actions) {
	while (GlobalStatistic.run) {
		int delay = DEFAULT_DELAY_IN_WHILE_CYCLE;

		if (GetAsyncKeyState('Z') & 0x0001) {
			Actions->RotateLeft = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState('X') & 0x0001 || GetAsyncKeyState(VK_UP) & 0x0001) {
			Actions->RotateRight = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x0001) {
			Actions->MoveLeft = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x0001) {
			Actions->MoveRight = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x0001) {
			*down_delay = HARD_DROP_DELAY;
			condition->notify_one(); 
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x0001) {
			*down_delay = SOFT_DROP_DELAY;
			condition->notify_one();
		}
		if (GetAsyncKeyState(VK_RSHIFT) & 0x0001 && GlobalStatistic.can_use_backpack) {
			Actions->UseBackpack = 1;
			GlobalStatistic.can_use_backpack = 0;
		}

		Sleep(delay);
	}
}

void DownFunc(int* down_delay, wchar_t** surface, mutex* mut, condition_variable* condition, Shape::Actions* Actions) {
	std::unique_lock<std::mutex> lock(*mut);

	*down_delay = BASE_DOWN_DELAY - GlobalStatistic.difficulty * CONVERT_DIFFYCULTY_TO_DELAY;

	while (GlobalStatistic.run) {
		if (*down_delay == HARD_DROP_DELAY) {
			GlobalStatistic.points += 2 * GlobalStatistic.difficulty;
		}

		if (*down_delay == SOFT_DROP_DELAY) {
			*down_delay = BASE_DOWN_DELAY - GlobalStatistic.difficulty * CONVERT_DIFFYCULTY_TO_DELAY;
			GlobalStatistic.points++;
		}

		condition->wait_for(lock, std::chrono::milliseconds(*down_delay));
		Actions->MoveDown = 1;
	}
}

ShapesBasket::ShapesBasket(){
	root = nullptr;
	size = 0;
	GenerateShapeList();
}

ShapesBasket::~ShapesBasket(){
	while (!IsEmpty()){
		RemoveShapeFromList();
	}
}

void ShapesBasket::AddShapeToList(int index) {
	size++;
	ShapeList* new_shape = new ShapeList;
	new_shape->shape = Shape(index);
	new_shape->p_next = root;
	root = new_shape;
}

void ShapesBasket::GenerateShapeList() {
	int different_shapes[7] = { 0, 1, 2, 3, 4, 5, 6 };
	for (int i = 0; i < 7; i++) {
		int j = rand() % 7;

		int tmp = different_shapes[i];
		different_shapes[i] = different_shapes[j];
		different_shapes[j] = tmp;
	}
	for (int i = 0; i < 7; i++) {
		this->AddShapeToList(different_shapes[i]);
	}
}

Shape ShapesBasket::RemoveShapeFromList() {
	size--;
	ShapeList* tmp = root;
	root = root->p_next;

	Shape shape_for_return = tmp->shape;
	delete tmp;

	return shape_for_return;
}

int ShapesBasket::IsEmpty(){
	return !size;
}

void Shape::AddShadowToSurface(wchar_t** surface_with_shape_and_shadow) {
	Shape current_shape = *this;

	wchar_t** tmp = new wchar_t* [SURFACE_WIDTH];

	for (int count = 0; count < SURFACE_WIDTH; count++) {
		tmp[count] = new wchar_t[SURFACE_HEIGHT];
	}

	for (int x = 0; x < SURFACE_WIDTH; x++)
		wmemcpy(tmp[x], surface_with_shape_and_shadow[x], SURFACE_HEIGHT);

	do {
		current_shape.MoveDown();
	} while (!current_shape.AddShapeToSurface(tmp, NO_SYMBOL));
	current_shape.MoveUp();

	current_shape.AddShapeToSurface(surface_with_shape_and_shadow, SHADOWS_SYMBOL);

	for (int count = 0; count < SURFACE_WIDTH; count++)
		delete[] tmp[count];

	delete[] tmp;
}

int Shape::AddShapeToSurface(wchar_t** surface, wchar_t sym) {
	wchar_t** tmp = new wchar_t* [SURFACE_WIDTH];

	for (int count = 0; count < SURFACE_WIDTH; count++) {
		tmp[count] = new wchar_t[SURFACE_HEIGHT];
	}

	for (int x = 0; x < SURFACE_WIDTH; x++) {
		wmemcpy(tmp[x], surface[x], SURFACE_HEIGHT);
	}

	for (int i = 0; i < 4; i++) {
		if (0 <= position.x + pixel_coordinates[i].x &&
			position.x + pixel_coordinates[i].x < SURFACE_WIDTH &&
			position.y + pixel_coordinates[i].y < SURFACE_HEIGHT &&
			surface[position.x + pixel_coordinates[i].x][position.y + pixel_coordinates[i].y] != MAIN_SYMBOL) {
			if (0 <= position.y + pixel_coordinates[i].y)
				surface[position.x + pixel_coordinates[i].x][position.y + pixel_coordinates[i].y] = sym;
		}
		else {
			for (int x = 0; x < SURFACE_WIDTH; x++)
				wmemcpy(surface[x], tmp[x], SURFACE_HEIGHT);

			for (int count = 0; count < SURFACE_WIDTH; count++)
				delete[] tmp[count];

			delete[] tmp;

			return -1;
		}
	}

	for (int count = 0; count < SURFACE_WIDTH; count++)
		delete[] tmp[count];

	delete[] tmp;

	return 0;
}

void Shape::generate() {
	switch (shape_index) {
	case 0:
		//OO
		//OO
		shape_array[0][0] = 1;
		shape_array[0][1] = 1;
		shape_array[1][0] = 1;
		shape_array[1][1] = 1;
		break;
	case 1:
		//OO
		//O
		//O
		shape_array[0][0] = 1;
		shape_array[0][1] = 1;
		shape_array[1][0] = 1;
		shape_array[2][0] = 1;
		break;
	case 2:
		//OO
		// O
		// O
		shape_array[0][0] = 1;
		shape_array[0][1] = 1;
		shape_array[1][1] = 1;
		shape_array[2][1] = 1;
		break;
	case 3:
		//O
		//O
		//O
		//O
		shape_array[0][1] = 1;
		shape_array[1][1] = 1;
		shape_array[2][1] = 1;
		shape_array[3][1] = 1;
		break;
	case 4:
		//OO
		// OO
		shape_array[0][0] = 1;
		shape_array[0][1] = 1;
		shape_array[1][1] = 1;
		shape_array[1][2] = 1;
		break;
	case 5:
		// OO
		//OO
		shape_array[0][1] = 1;
		shape_array[0][2] = 1;
		shape_array[1][0] = 1;
		shape_array[1][1] = 1;
		break;
	case 6:
		// O 
		//OOO
		shape_array[0][1] = 1;
		shape_array[1][0] = 1;
		shape_array[1][1] = 1;
		shape_array[1][2] = 1;
		break;
	}
}
Shape::Shape(int index) {
	NullShape();

	if (index < 0)
		shape_index = rand() % 7;
	else
		shape_index = index;

	position.x = BASE_SHAPE_X;
	position.y = BASE_SHAPE_Y;

	x_center = 1;
	y_center = 1;

	generate();
	ArrayToCoords();
}

void Shape::NullShape() {
	memset(pixel_coordinates, 0x00, 4 * sizeof(pixel));

	memset(shape_array, 0x00, 4 * 4 * sizeof(int));

	position.x = -1;
	position.y = -1;

	x_center = -1;
	y_center = -1;

	shape_index = -1;
}

Shape::~Shape() {
	return;
}

void Shape::rotate_right(wchar_t** surface) {
	int tmp[4][4];

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			tmp[y][(4 - 1) - x] = shape_array[x][y];
		}
	}

	if (x_center == y_center) {
		x_center = (1 + 2) - x_center;
	}
	else {
		y_center = (1 + 2) - y_center;
	}

	Shape tmp_shape = *this;
	memcpy(tmp_shape.shape_array, tmp, 4 * 4 * sizeof(int));
	tmp_shape.ArrayToCoords();

	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}

	tmp_shape.position.x++;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x++;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}
	tmp_shape.position.x++;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x += 2;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}

	tmp_shape.position.x -= 2;

	tmp_shape.position.x--;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x--;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}
	tmp_shape.position.x--;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x -= 2;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}
}

void Shape::rotate_left(wchar_t** surface) {
	int tmp[4][4];

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			tmp[(4 - 1) - y][x] = shape_array[x][y];
		}
	}

	if (x_center == y_center) {
		y_center = (1 + 2) - y_center;
	}
	else {
		x_center = (1 + 2) - x_center;
	}

	Shape tmp_shape = *this;
	memcpy(tmp_shape.shape_array, tmp, 4 * 4 * sizeof(int));
	tmp_shape.ArrayToCoords();

	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}

	tmp_shape.position.x++;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x++;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}
	tmp_shape.position.x++;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x += 2;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}

	tmp_shape.position.x -= 2;

	tmp_shape.position.x--;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x--;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}
	tmp_shape.position.x--;
	if (!tmp_shape.AddShapeToSurface(surface, NO_SYMBOL)) {
		position.x -= 2;
		memcpy(shape_array, tmp, 4 * 4 * sizeof(int));
		ArrayToCoords();
		return;
	}
}

void Shape::MoveRight(wchar_t** surface) {
	int max_x = -1;
	for (int i = 0; i < 4; i++) {
		if (max_x < pixel_coordinates[i].x)
			max_x = pixel_coordinates[i].x;
	}

	position.x++;

	wchar_t** tmp = new wchar_t* [SURFACE_WIDTH];

	for (int count = 0; count < SURFACE_WIDTH; count++) {
		tmp[count] = new wchar_t[SURFACE_HEIGHT];
	}

	for (int x = 0; x < SURFACE_WIDTH; x++)
		wmemcpy(tmp[x], surface[x], SURFACE_HEIGHT);

	if (max_x + position.x > SURFACE_WIDTH - 1 || AddShapeToSurface(tmp, MAIN_SYMBOL)) {
		position.x--;
	}

	for (int count = 0; count < SURFACE_WIDTH; count++)
		delete[] tmp[count];

	delete[] tmp;

}

void Shape::MoveLeft(wchar_t** surface) {
	int min_x = 1111;
	for (int i = 0; i < 4; i++) {
		if (min_x > pixel_coordinates[i].x)
			min_x = pixel_coordinates[i].x;
	}

	position.x--;

	wchar_t** tmp = new wchar_t* [SURFACE_WIDTH];

	for (int count = 0; count < SURFACE_WIDTH; count++) {
		tmp[count] = new wchar_t[SURFACE_HEIGHT];
	}

	for (int x = 0; x < SURFACE_WIDTH; x++)
		wmemcpy(tmp[x], surface[x], SURFACE_HEIGHT);

	if (min_x + position.x < 0 || AddShapeToSurface(tmp, MAIN_SYMBOL)) {
		position.x++;
	}

	for (int count = 0; count < SURFACE_WIDTH; count++)
		delete[] tmp[count];

	delete[] tmp;
}

void Shape::MoveUp() {
	this->position.y--;
}

void Shape::MoveDown() {
	this->position.y++;
}

void Shape::ArrayToCoords() {
	//In array 4x4 [1][1] - center of the shape
	int i = 0;

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (shape_array[y][x] == 1) {
				pixel_coordinates[i].x = x - x_center; //[1][1] - center, so shape_array[0][0] - (-1, -1) - in pixel_coordinates
				pixel_coordinates[i].y = y - y_center; //
				i++;
			}
		}
	}
}

bool Shape::operator==(const Shape& other) {
	int result = 1;

	result &= shape_index == other.shape_index;
	result &= x_center == other.x_center;
	result &= y_center == other.y_center;
	result &= position.x == other.position.x;
	result &= position.y == other.position.y;

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			result &= shape_array[x][y] == other.shape_array[x][y];
		}
	}

	for (int i = 0; i < 4; i++) {
		result &= pixel_coordinates[i].x== other.pixel_coordinates[i].x;
		result &= pixel_coordinates[i].y== other.pixel_coordinates[i].y;
	}

	return result;
}

void Shape::Statistics(wchar_t* screen, int left_up_x, int left_up_y, Shape& next_shape, Shape& backpack_shape) {
	wchar_t time_text[STATISTICS_LINE_LENGTH] = L"Time: XX:XX    ";
	wchar_t next_shape_text[STATISTICS_LINE_LENGTH] = L"Backpack shape:";
	wchar_t backpack_shape_text[STATISTICS_LINE_LENGTH] = L"Next shape:    ";
	wchar_t rows_text[STATISTICS_LINE_LENGTH] = L"Rows: XXXXXXXX ";
	wchar_t points_text[STATISTICS_LINE_LENGTH] = L"Points: XXXXXX ";
	wchar_t difficulty_text[STATISTICS_LINE_LENGTH] = L"Difficulty: XX ";

	GlobalStatistic.current_time = time(NULL);
	GlobalStatistic.elapsed_time = GlobalStatistic.current_time - GlobalStatistic.game_start_time;

	time_text[6] = (GlobalStatistic.elapsed_time) / 60 % 60 / 10 + L'0';
	time_text[7] = (GlobalStatistic.elapsed_time) / 60 % 60 % 10 + L'0';
	time_text[9] = (GlobalStatistic.elapsed_time) % 60 / 10 + L'0';
	time_text[10] = (GlobalStatistic.elapsed_time) % 60 % 10 + L'0';

	int flag = 0;
	for (int i = 6; i < 14; i++) {
		int res = GlobalStatistic.rows % (int)pow((int)10, (int)(14 - i)) / (int)pow((int)10, (int)(13 - i));

		if (flag || res || i == 13)
			rows_text[i] = res + L'0';
		else
			rows_text[i] = NO_SYMBOL;

		if (res) flag = 1;
	}

	flag = 0;
	for (int i = 8; i < 14; i++) {
		int res = GlobalStatistic.points % (int)pow((int)10, (int)(14 - i)) / (int)pow((int)10, (int)(13 - i));

		if (flag || res || i == 13)
			points_text[i] = res + L'0';
		else
			points_text[i] = NO_SYMBOL;

		if (res) flag = 1;
	}

	flag = 0;
	for (int i = 12; i < 14; i++) {
		int res = GlobalStatistic.difficulty % (int)pow((int)10, (int)(14 - i)) / (int)pow((int)10, (int)(13 - i));

		if (flag || res || i == 13)
			difficulty_text[i] = res + L'0';
		else
			difficulty_text[i] = NO_SYMBOL;

		if (res) flag = 1;
	}


	memcpy(&screen[(left_up_y + 1) * SCREEN_WIDTH + (left_up_x + 1)], time_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));

	memcpy(&screen[(left_up_y + 3) * SCREEN_WIDTH + (left_up_x + 1)], next_shape_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			screen[(left_up_y + 4 + y) * SCREEN_WIDTH + (left_up_x + 6 + x)] = (next_shape.shape_array[y][x]) ? MAIN_SYMBOL : NO_SYMBOL;
		}
	}

	memcpy(&screen[(left_up_y + 9) * SCREEN_WIDTH + (left_up_x + 1)], backpack_shape_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			screen[(left_up_y + 10 + y) * SCREEN_WIDTH + (left_up_x + 6 + x)] = (backpack_shape.shape_array[y][x]) ? MAIN_SYMBOL : NO_SYMBOL;
		}
	}

	memcpy(&screen[(left_up_y + 15) * SCREEN_WIDTH + (left_up_x + 1)], rows_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	memcpy(&screen[(left_up_y + 17) * SCREEN_WIDTH + (left_up_x + 1)], points_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));
	memcpy(&screen[(left_up_y + 19) * SCREEN_WIDTH + (left_up_x + 1)], difficulty_text, STATISTICS_LINE_LENGTH * sizeof(wchar_t));

	return;
}

int Shape::GetY() {
	return position.y;
}

void Shape::MoveShape(Actions& Actions, Shape& backpack_shape, wchar_t** surface) {
	if (Actions.RotateLeft) {
		//rotate_left(surface);
		
		rotate_right(surface);
		rotate_right(surface);
		rotate_right(surface);
	}

	if (Actions.RotateRight) {
		rotate_right(surface);
	}

	if (Actions.MoveLeft) {
		MoveLeft(surface);
	}

	if (Actions.MoveRight) {
		MoveRight(surface);
	}

	if (Actions.MoveDown) {
		if(!AddShapeToSurface(surface, NO_SYMBOL))
			MoveDown();
	}

	if (Actions.UseBackpack) {
		int backpack_index = backpack_shape.shape_index;
		int current_index = shape_index;

		*this = Shape(backpack_index);
		backpack_shape = Shape(current_index);
	}

	Actions.MoveUp = 0;
	Actions.MoveDown = 0;
	Actions.MoveLeft = 0;
	Actions.MoveRight = 0;
	Actions.RotateLeft = 0;
	Actions.RotateRight = 0;
	Actions.UseBackpack = 0;
}