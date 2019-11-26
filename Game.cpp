
#include "Game.h"
#include "Player.h"

/* GAME CLASS DEFINITIONS */

void Game::play(UI* ui) {

	// Local Vars
	Gameboard	board;
	int			curPlayer = 0;				// current player in game
	bool		done = false;				// If Game has finished [win/loss/draw occurred]
	std::vector<Gameboard::Option> moves;	// List of moves current player can make

	// 1) Init Players, Pieces
	Player players[] = {Player(Color::WHITE), Player(Color::BLACK)};
	Piece* kings[2];

	players[0].create(Piece::Type::ROOK, glm::vec2(0, 0), board);
	players[0].create(Piece::Type::KNIGHT, glm::vec2(0, 1), board);
	players[0].create(Piece::Type::BISHOP, glm::vec2(0, 2), board);
	players[0].create(Piece::Type::QUEEN, glm::vec2(0, 3), board);
	kings[0] = players[0].create(Piece::Type::KING, glm::vec2(0, 4), board);
	players[0].create(Piece::Type::BISHOP, glm::vec2(0, 5), board);
	players[0].create(Piece::Type::KING, glm::vec2(0, 6), board);
	players[0].create(Piece::Type::ROOK, glm::vec2(0, 7), board);
	for (int i = 0; i < board.COL; i++) {
		players[0].create(Piece::Type::PAWN, glm::vec2(1, i), board);
	}

	players[1].create(Piece::Type::ROOK, glm::vec2(7, 0), board);
	players[1].create(Piece::Type::KNIGHT, glm::vec2(7 ,1), board);
	players[1].create(Piece::Type::BISHOP, glm::vec2(7, 2), board);
	players[1].create(Piece::Type::QUEEN, glm::vec2(7, 3), board);
	kings[1] = players[1].create(Piece::Type::KING, glm::vec2(7, 4), board);
	players[1].create(Piece::Type::BISHOP, glm::vec2(7, 5), board);
	players[1].create(Piece::Type::KNIGHT, glm::vec2(7, 6), board);
	players[1].create(Piece::Type::ROOK, glm::vec2(7, 7), board);
	for (int i = 0; i < board.COL; i++) {
		players[1].create(Piece::Type::PAWN, glm::vec2(6, i), board);
	}

	ui->drawBoard(board);

	// Begin Game Loop
	while (!done) {

		// 2) Generate list of moves for current player
		moves = board.generateOptions(players[curPlayer].getOwned(), *(kings[curPlayer]));

		// 3) Test to see if king is in check
	}
}