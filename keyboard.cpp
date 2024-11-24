#include "keyboard.hpp"

#include <windows.h>
#include <ctime>
#include <thread>
#include <winuser.h>

#pragma comment (lib, "User32.lib")

Keyboard::Keyboard(){
	return;
}

Keyboard::~Keyboard(){
	return;
}

void Keyboard::MoveFunc(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic) {
	while (statistic->run) {
		int delay = DEFAULT_DELAY_IN_WHILE_CYCLE;

		if (GetAsyncKeyState('Z') & 0x0001) {
			actions->RotateLeft = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState('X') & 0x0001 || GetAsyncKeyState(VK_UP) & 0x0001) {
			actions->RotateRight = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x0001) {
			actions->MoveLeft = 1;

			delay = BASE_STREAM_MOVE_DELAY;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x0001) {
			actions->MoveRight = 1;

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
		if (GetAsyncKeyState(VK_RSHIFT) & 0x0001 && statistic->can_use_backpack) {
			actions->UseBackpack = 1;
			statistic->can_use_backpack = 0;
		}

		Sleep(delay);
	}
}

void Keyboard::WaitAction() {
	while (1) {
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
			break;
		Sleep(DEFAULT_DELAY_IN_WHILE_CYCLE);
	}
}

void Keyboard::TrackActions(int* down_delay, std::mutex* mut, std::condition_variable* condition, Actions* actions, GlobalStatistic* statistic){
	std::thread th(&Control::MoveFunc, *this, down_delay, mut, condition, actions, statistic);
	th.detach();
}