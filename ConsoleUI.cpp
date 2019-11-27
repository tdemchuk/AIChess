
#include "ConsoleUI.h"
#include <iostream>


void ConsoleUI::drawBoard(const Gameboard& board) const
{
	Piece* curPiece;
	char teamLetter;

	std::cout << '\n';

	for (int r = board.ROW-1; r >= 0; r--) {

		std::cout << " " << (r + 1) << " | ";		// print rank

		for (int c = 0; c < board.COL; c++) {

			curPiece = board.check(glm::vec2(r,c));

			if (curPiece) {

				if (curPiece->team() == Color::WHITE)	teamLetter = 'W';
				else									teamLetter = 'B';

				std::cout << teamLetter;

				switch (curPiece->type()) {
				case Piece::Type::BISHOP:
					std::cout << 'B';
					break;
				case Piece::Type::KING:
					std::cout << 'K';
					break;
				case Piece::Type::KNIGHT:
					std::cout << 'N';
					break;
				case Piece::Type::PAWN:
					std::cout << 'P';
					break;
				case Piece::Type::QUEEN:
					std::cout << 'Q';
					break;
				case Piece::Type::ROOK:
					std::cout << 'R';
					break;
				}

				std::cout << ' ';
			}
			else {
				std::cout << "## ";
			}
		}

		std::cout << '\n';
	}

	std::cout << "    ";
	for (int i = 0; i < board.COL; i++) {
		std::cout << "---";
	}
	std::cout << "\n     ";
	for (int i = 0; i < board.COL; i++) {
		std::cout << (char)(i + 97) << "  ";
	}

	std::cout << "\n\n";
}

// Prompts the current player to make a move
void ConsoleUI::promptMove(std::vector<Gameboard::Playable>& moves) 
{
	// Local Vars
	Piece* cur = nullptr;

	std::cout << "\nSelect a Piece To Move [Enter Index From Following List] :\n";

	for (int i = 0; i < moves.size(); i++) {
		cur = moves[i].piece;
		std::cout << i << " : " << cur->teamStr() << " " << cur->typeStr() << " at [" << cur->getPos().x << "," << cur->getPos().y << "]\n";
	}
}

// Prompts the current player to choose what to promote a piece to 
void ConsoleUI::promptPromote() {}

// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
void ConsoleUI::promptGameMode() {}

// Draws the provided message string to the screen (use for win/loss/draw messages)
void ConsoleUI::drawMessage(std::string msg) {}