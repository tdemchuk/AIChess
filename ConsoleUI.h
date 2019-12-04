#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "UI.h"


class ConsoleUI : public UI
{
public:
	// Draws the current gameboard to the screen
	void drawBoard(const Gameboard& board) const override;

	// Prompts the current player to make a move
	glm::vec2 promptMove(std::vector<Gameboard::Playable>& moves) override;

	// Prompts the current player to choose what to promote a piece to 
	Piece::Type promptPromote() override;

	// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
	Mode promptGameMode() override;

	// Draws the provided message string to the screen (use for win/loss/draw messages)
	void drawMessage(std::string msg) override;

private:

};

#endif