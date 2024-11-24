#include "tetris.hpp"
#include "config.hpp"

int main(void) {
	srand(time(NULL));
	Config conf;
	Tetris game;

	game.Run(conf.GetScreen(), conf.GetController());
}
