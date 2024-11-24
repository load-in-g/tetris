#ifndef _CONTROL_INT
#define _CONTROL_INT

#include "consts_and_structs.hpp"
#include <windows.h>
#include <future>

class Control {
public:
	virtual ~Control() {}

	virtual void WaitAction() = 0;
	virtual void MoveFunc(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic) = 0;

	virtual void TrackActions(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic) = 0;
};

#endif