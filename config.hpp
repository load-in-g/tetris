#ifndef _CONFIG
#define _CONFIG

#include "gui.hpp"
#include "control.hpp"

#include "consoleinterface.hpp"
#include "keyboard.hpp"

class Config{
public:
	Config();
	~Config();
	ConsoleInterface& GetScreen();
	Keyboard& GetController();

private:
	ConsoleInterface* screen;
	Keyboard* keyboard_controller;
};

#endif