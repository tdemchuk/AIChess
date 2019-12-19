
//#define GUI_ON		// Uncomment to use experimental GUI. Otherwise console-based UI will be used ||| **GUI UNFINISHED**


#include "Game.h"
#include "Gameboard.h"
#include "UI.h"
#include "ConsoleUI.h"
#include "Gamemode.h"
#include <iostream>



int main(int argc, char* argv[])
{
#ifndef GUI_ON	// NO GUI
	UI* ui = new ConsoleUI();
#endif
	
	// TODO - Add prompt for Game Mode/Difficulty Here, pass into Game's play() function
	Mode mode = ui->promptGameMode();

	// Launch Game
	Game game;
	game.play(ui, mode);

	delete ui;

	return 0;
}