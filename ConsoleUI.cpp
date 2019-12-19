
#include "ConsoleUI.h"
#include <iostream>


// Draws the current board state of the game to the console
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


	std::cout << "\nEnter '-1' To Forfeit The Game\n";
	std::cout << "\nSelect a Piece To Move [Enter Index From Following List] :\n";

	for (int i = 0; i < moves.size(); i++) {
		cur = moves[i].piece;
		std::cout << i << " : " << cur->teamStr() << " " << cur->typeStr() << " at [" << cur->rank() << "," << cur->file() << "]\n";
	}

	do {
		if (input != 0) std::cout << "Invalid Move";
		std::cout << "\n--> ";
		if (!(std::cin >> input)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			input = -9999;
		}
		std::cout << "\n";
		if (input == -1) {
			chosen.x = -1;
			return chosen;
		}
	} while (input < 0 || input >= moves.size());

	chosen.x = input;
	if (input == -1) return chosen;

	std::cout << "\nSelect a Move To Make [Enter Index From Following List] :\n";

	for (int i = 0; i < moves[chosen.x].moves.size(); i++) {
		std::cout << i << " : [" << (moves[chosen.x].moves[i].coord.x + 1) << "," << (char)(moves[chosen.x].moves[i].coord.y + 97) << "]\n";
	}

	input = 0;
	do {
		if (input != 0) std::cout << "Invalid Move";
		std::cout << "\n--> ";
		if (!(std::cin >> input)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			input = -9999;
		}
		std::cout << "\n";
		if (input == -1) {
			chosen.x = -1;
			return chosen;
		}
	} while (input < 0 || input >= moves[chosen.x].moves.size());

	chosen.y = input;

	return chosen;
}

// Prompts the current player to choose what to promote a piece to 
Piece::Type ConsoleUI::promptPromote() 
{
	// Local Vars
	int input = 0;

	std::cout << "\nWhat Do You Want to Promote This Pawn To? [Enter Index From Following List] :\n";
	std::cout << "0 : Queen\n1 : Bishop\n2 : Knight\n3 : Rook\n";
	
	do {
		if (input != 0) std::cout << "Invalid Input";
		std::cout << "\n--> ";
		if (!(std::cin >> input)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			input = -9999;
		}
		std::cout << "\n";
	} while (input < 0 || input > 3);

	switch (input) {
	case 0: return Piece::Type::QUEEN;
	case 1: return Piece::Type::BISHOP;
	case 2: return Piece::Type::KNIGHT;
	case 3: return Piece::Type::ROOK;
	default: return Piece::Type::QUEEN;
	}
}

// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
Mode ConsoleUI::promptGameMode() 
{
	// Local Vars
	int input = 1;
	Mode mode;
	
	std::cout << "\nSelect The Number Of Players: \n";
	std::cout << "1 : 1 Player\n2 : 2 Player\n";

	do {
		if (input != 1) std::cout << "Invalid Input";
		std::cout << "\n--> ";
		if (!(std::cin >> input)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			input = -9999;
		}
		std::cout << "\n";
	} while (input < 1 || input > 2);

	mode.numPlayers = input;

	if (input == 2) return mode;	// two player mode doesnt require a difficulty specification

	// TODO : Make prompt for difficulty when playing against AI
	std::cout << "Select AI Difficulty [Enter Difficulty From Following List] :\n";
	std::cout << "0 : EASY\n1 : MEDIUM\n2 : HARD\n";

	input = 0;

	do {
		if (input != 0) std::cout << "Invalid Input";
		std::cout << "\n--> ";
		if (!(std::cin >> input)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			input = -9999;
		}
		std::cout << "\n";
	} while (input < 0 || input > 2);

	mode.diff = (Difficulty)input;

	return mode;
}

// Draws the provided message string to the screen (use for win/loss/draw messages)
void ConsoleUI::drawMessage(std::string msg) 
{
	std::cout << msg;
}

void ConsoleUI::drawCaptured(Player(&players)[2])
{
	// Draw Player 1 Captured List to screen
	std::cout << "Player 1 (WHITE) Captured : ";
	for (int i = 0; i < players[0].getCaptured().size(); i++) {
		if (i != 0) std::cout << ", ";
		std::cout << players[0].getCaptured()[i]->typeStr();
	}
	std::cout << '\n';

	// Draw Player 2 Captured List to screen
	std::cout << "Player 2 (BLACK) Captured : ";
	for (int i = 0; i < players[1].getCaptured().size(); i++) {
		if (i != 0) std::cout << ", ";
		std::cout << players[1].getCaptured()[i]->typeStr();
	}
	std::cout << '\n';
}