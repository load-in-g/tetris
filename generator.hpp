#ifndef _GENERATOR
#define _GENERATOR

#include "shape.hpp"

class Generator {
public:
	Generator();
	~Generator();
	void AddShapeToList(int index);
	void GenerateShapeList();
	Shape RemoveShapeFromList();
	int IsEmpty();

private:
	int size;
	struct ShapeList {
		Shape shape;
		ShapeList* p_next;
	};
	ShapeList* root;
};

#endif