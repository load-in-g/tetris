#include "generator.hpp"
#include <cmath>

Generator::Generator() {
	root = nullptr;
	size = 0;
	GenerateShapeList();
}

Generator::~Generator() {
	while (!IsEmpty()) {
		size--;
		ShapeList* tmp = root;
		root = root->p_next;

		delete tmp;
	}
	root = nullptr;
	size = 0;
}

void Generator::AddShapeToList(int index) {
	size++;
	ShapeList* new_shape = new ShapeList;
	new_shape->shape = Shape(index);
	new_shape->p_next = root;
	root = new_shape;
}

void Generator::GenerateShapeList() {
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

Shape Generator::RemoveShapeFromList() {
	size--;
	ShapeList* tmp = root;
	root = root->p_next;

	Shape shape_for_return = tmp->shape;
	delete tmp;

	if (this->IsEmpty())
		this->GenerateShapeList();

	return shape_for_return;
}

int Generator::IsEmpty() {
	return !size;
}