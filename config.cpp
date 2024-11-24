#include "config.hpp"

Config::Config(){
	this->screen = new ConsoleInterface;
	this->keyboard_controller = new Keyboard;
}

Config::~Config(){
	delete this->screen;
	delete this->keyboard_controller;
}

ConsoleInterface& Config::GetScreen(){
	return *this->screen;
}

Keyboard& Config::GetController(){
	return *this->keyboard_controller;
}