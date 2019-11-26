
#include "Game.h"
#include "Gameboard.h"
#include "UI.h"
#include "ConsoleUI.h"
#include <iostream>

//#define GUI_ON		// Uncomment to use experimental GUI. Otherwise console-based UI will be used


int main(int argc, char* argv[])
{
	// TODO - Init UI Here, pass into Game's play() function
#ifndef GUI_ON
	UI* ui = new ConsoleUI();
#endif
	// TODO - Add prompt for Game Difficulty Here, pass into Game's play() fucntion

	// Launch Game
	Game game;
	game.play(ui);

	delete ui;

	return 0;
}