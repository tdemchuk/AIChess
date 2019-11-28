
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
// @return The indices corresponding to the chosen piece/move
glm::vec2 ConsoleUI::promptMove(std::vector<Gameboard::Playable>& moves)
{
	// Local Vars
	Piece* cur = nullptr;
	glm::vec2 chosen;
	int input = 0;

	std::cout << "\nSelect a Piece To Move [Enter Index From Following List] :\n";

	for (int i = 0; i < moves.size(); i++) {
		cur = moves[i].piece;
		std::cout << i << " : " << cur->teamStr() << " " << cur->typeStr() << " at [" << cur->rank() << "," << cur->file() << "]\n";
	}

	do {
		if (input != 0) std::cout << "Invalid Move";
		std::cout << "\n--> ";
		std::cin >> input;
		std::cout << "\n";
	} while (input < 0 || input >= moves.size());

	chosen.x = input;

	std::cout << "\nSelect a Move To Make [Enter Index From Following List] :\n";

	for (int i = 0; i < moves[chosen.x].moves.size(); i++) {
		std::cout << i << " : [" << (moves[chosen.x].moves[i].coord.x + 1) << "," << (char)(moves[chosen.x].moves[i].coord.y + 97) << "]\n";
	}

	input = 0;
	do {
		if (input != 0) std::cout << "Invalid Move";
		std::cout << "\n--> ";
		std::cin >> input;
		std::cout << "\n";
	} while (input < 0 || input >= moves[chosen.x].moves.size());

	chosen.y = input;

	return chosen;
}

// Prompts the current player to choose what to promote a piece to 
void ConsoleUI::promptPromote() {}

// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
void ConsoleUI::promptGameMode() {}

// Draws the provided message string to the screen (use for win/loss/draw messages)
void ConsoleUI::drawMessage(std::string msg) {}