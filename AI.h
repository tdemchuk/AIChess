#ifndef AI_H
#define AI_H

#include "Game.h"
#include "Player.h"
#include <vector>
#include <iostream>

/*
	Class that provides function to intialize a Chess AI
*/
class AI
{
public:

	AI() = delete;
	AI(int ply);
	~AI() = default;

	const int Ply() const;																						//return the AI's Ply
	glm::vec2 ABPrune(Gameboard& board, Player (&players)[2], Piece* (&kings)[2], int currentPlayer, int depth);			//Perform AB Prune based off current board state
	int EvalHeuristic(Gameboard& board);																		//Evaluate the Heuristic score of a given gameBoard



private:
	const int m_Ply;
};

#endif