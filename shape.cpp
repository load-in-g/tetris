#include "shape.hpp"
#include <windows.h>

void Shape::AddShadowToSurface(Surface& surface_with_shape_and_shadow) {
	Shape current_shape = *this;

	Surface tmp;
	tmp = surface_with_shape_and_shadow;

	do {
		current_shape.MoveDown();
	} while (!current_shape.AddShapeToSurface(tmp, NO_SYMBOL));
	current_shape.MoveUp();

	current_shape.AddShapeToSurface(surface_with_shape_and_shadow, SHADOWS_SYMBOL);
}

int Shape::AddShapeToSurface(Surface& surface, wchar_t sym) {
	Surface tmp;
	tmp = surface;

	for (int i = 0; i < 4; i++) {
		if (0 <= position.x + pixel_coordinates[i].x &&
			position.x + pixel_coordinates[i].x < SURFACE_WIDTH &&
			position.y + pixel_coordinates[i].y < SURFACE_HEIGHT &&
			surface.GetSurface(position.x + pixel_coordinates[i].x, position.y + pixel_coordinates[i].y) != MAIN_SYMBOL) {
			if (0 <= position.y + pixel_coordinates[i].y)
				surface.SetSurface(position.x + pixel_coordinates[i].x, position.y + pixel_coordinates[i].y, sym);
		}
		else {
			surface = tmp;

			return -1;
		}
	}

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

	center_pixel.x = 1;
	center_pixel.y = 1;

	generate();
	ArrayToCoords();
}

void Shape::NullShape() {
	memset(this->pixel_coordinates, 0x00, 4 * sizeof(pixel));

	memset(this->shape_array, 0x00, 4 * 4 * sizeof(int));

	position.x = -1;
	position.y = -1;

	center_pixel.x = -1;
	center_pixel.y = -1;

	shape_index = -1;
}

Shape::~Shape() {
	return;
}

void Shape::rotate_right(Surface& surface) {
	int tmp[4][4];

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			tmp[y][(4 - 1) - x] = shape_array[x][y];
		}
	}

	if (center_pixel.x == center_pixel.y) {
		center_pixel.x = (1 + 2) - center_pixel.x;
	}
	else {
		center_pixel.y = (1 + 2) - center_pixel.y;
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

void Shape::rotate_left(Surface& surface) {
	int tmp[4][4];

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			tmp[(4 - 1) - y][x] = shape_array[x][y];
		}
	}

	if (center_pixel.x == center_pixel.y) {
		center_pixel.y = (1 + 2) - center_pixel.y;
	}
	else {
		center_pixel.x = (1 + 2) - center_pixel.x;
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

void Shape::MoveRight(Surface& surface) {
	int max_x = -1;
	for (int i = 0; i < 4; i++) {
		if (max_x < pixel_coordinates[i].x)
			max_x = pixel_coordinates[i].x;
	}

	position.x++;

	Surface tmp;
	tmp = surface;

	if (max_x + position.x > SURFACE_WIDTH - 1 || AddShapeToSurface(tmp, MAIN_SYMBOL))
		position.x--;
}

void Shape::MoveLeft(Surface& surface) {
	int min_x = 1111;
	for (int i = 0; i < 4; i++) {
		if (min_x > pixel_coordinates[i].x)
			min_x = pixel_coordinates[i].x;
	}

	position.x--;

	Surface tmp;
	tmp = surface;

	if (min_x + position.x < 0 || AddShapeToSurface(tmp, MAIN_SYMBOL))
		position.x++;
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
				pixel_coordinates[i].x = x - center_pixel.x; //[1][1] - center, so shape_array[0][0] - (-1, -1) - in pixel_coordinates
				pixel_coordinates[i].y = y - center_pixel.y; //
				i++;
			}
		}
	}
}

bool Shape::operator==(const Shape& other) {
	int result = 1;

	result &= shape_index == other.shape_index;
	result &= center_pixel.x == other.center_pixel.x;
	result &= center_pixel.y == other.center_pixel.y;
	result &= position.x == other.position.x;
	result &= position.y == other.position.y;

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			result &= shape_array[x][y] == other.shape_array[x][y];
		}
	}

	for (int i = 0; i < 4; i++) {
		result &= pixel_coordinates[i].x == other.pixel_coordinates[i].x;
		result &= pixel_coordinates[i].y == other.pixel_coordinates[i].y;
	}

	return result;
}

void Shape::MoveShape(Actions& actions, Shape& backpack_shape, Surface& surface) {
	if (actions.RotateLeft) {
		//rotate_left(surface);

		rotate_right(surface);
		rotate_right(surface);
		rotate_right(surface);
	}

	if (actions.RotateRight) {
		rotate_right(surface);
	}

	if (actions.MoveLeft) {
		MoveLeft(surface);
	}

	if (actions.MoveRight) {
		MoveRight(surface);
	}

	if (actions.MoveDown) {
		if (!AddShapeToSurface(surface, NO_SYMBOL))
			MoveDown();
	}

	if (actions.UseBackpack) {
		int backpack_index = backpack_shape.shape_index;
		int current_index = shape_index;

		*this = Shape(backpack_index);
		backpack_shape = Shape(current_index);
	}

	actions.MoveUp = 0;
	actions.MoveDown = 0;
	actions.MoveLeft = 0;
	actions.MoveRight = 0;
	actions.RotateLeft = 0;
	actions.RotateRight = 0;
	actions.UseBackpack = 0;
}

int Shape::GetArray(int x, int y) {
	return this->shape_array[x][y];
}