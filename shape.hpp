#ifndef _SHAPE
#define _SHAPE

#include "consts_and_structs.hpp"
#include "surface.hpp"

class Shape {
public:
	Shape(int index = -1);
	~Shape();
	void rotate_right(Surface& surface);
	void rotate_left(Surface& surface);
	void generate();
	void MoveRight(Surface& surface);
	void MoveLeft(Surface& surface);
	void MoveUp();
	void MoveDown();
	void ArrayToCoords();
	void NullShape();
	void MoveShape(Actions& actions, Shape& backpack_shape, Surface& surface);
	void AddShadowToSurface(Surface& surface_with_shape_and_shadow);
	int AddShapeToSurface(Surface& surface, wchar_t sym);

	bool operator==(const Shape& other);

	int GetArray(int x, int y);

private:
	pixel pixel_coordinates[4] = { -1 };
	int shape_array[4][4];
	int shape_index = -1;

	pixel position;
	pixel center_pixel = { 1, 1 };
};

#endif