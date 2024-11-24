#ifndef _KEYBOARD
#define _KEYBOARD

#include "control.hpp"

class Keyboard : public Control {
public:
	Keyboard();
	~Keyboard();
	void WaitAction();
	void MoveFunc(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic);

	void TrackActions(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic);
};

#endif