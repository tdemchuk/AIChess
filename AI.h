#ifndef AI_H
#define AI_H

#include "Game.h"
#include "Player.h"
#include <vector>
#include <iostream>

/*
	Class that provides function to play a Game of chess
*/
class AI
{
public:

	void makeMove(std::vector<Gameboard::Playable> AIplayables);


};

#endif