#ifndef _GUI
#define _GUI

#include "surface.hpp"
#include "shape.hpp"

class GUI {
public:
	virtual ~GUI() {}

	virtual void SetWindow(int Width, int Height) = 0;

	virtual void DrawBorders() = 0;

	virtual void SetStart() = 0;
	virtual void SetRestart() = 0;

	virtual void AddSurfaceToScreen(Surface& surface, int start_x, int start_y) = 0;
	virtual void Clear() = 0;
	virtual void Statistics(int left_up_x, int left_up_y, Shape& next_shape, Shape& backpack_shape, GlobalStatistic& statistic) = 0;

	virtual void Draw() = 0;
};

#endif