#ifndef UI_H
#define UI_H


#include "Gameboard.h"

/*
	UI Superclass - declares function prototypes used to interact with the user 
*/

class UI
{
public:
	
	// Draws the current gameboard to the screen
	virtual void drawBoard(const Gameboard& board) const = 0;

	// Prompts the current player to make a move
	virtual glm::vec2 promptMove(std::vector<Gameboard::Playable>& moves) = 0;

	// Prompts the current player to choose what to promote a piece to 
	virtual Piece::Type promptPromote() = 0;

	// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
	virtual void promptGameMode() = 0;

	// Draws the provided message string to the screen (use for win/loss/draw messages)
	virtual void drawMessage(std::string msg) = 0;

private:

};

#endif