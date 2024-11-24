#ifndef _SURFACE
#define _SURFACE

#include "consts_and_structs.hpp"

class Surface {
public:
	Surface();
	~Surface();

	void Clear();
	void RemoveLines(GlobalStatistic& statistic);
	void DropLines();

	wchar_t GetSurface(int x, int y);
	void SetSurface(int x, int y, wchar_t sym);

	Surface& operator=(const Surface& other);

private:
	wchar_t** _surface;
};

#endif