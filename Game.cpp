
#include "Game.h"
#include "Player.h"

/* GAME CLASS DEFINITIONS */

void Game::play() {

	// Local Vars
	int curPlayer = 0;	// start with Player 1

	// 1) Init Gameboard, Players, Pieces
	Gameboard board;
	Player players[] = {Player(Color::WHITE), Player(Color::BLACK)};

	players[0].create(Piece::Type::ROOK, glm::vec2(0, 0), board);
	players[0].create(Piece::Type::KNIGHT, glm::vec2(1, 0), board);
	players[0].create(Piece::Type::BISHOP, glm::vec2(2, 0), board);
	players[0].create(Piece::Type::QUEEN, glm::vec2(3, 0), board);
	players[0].create(Piece::Type::KING, glm::vec2(4, 0), board);
	players[0].create(Piece::Type::BISHOP, glm::vec2(5, 0), board);
	players[0].create(Piece::Type::KNIGHT, glm::vec2(6, 0), board);
	players[0].create(Piece::Type::ROOK, glm::vec2(7, 0), board);
	for (int i = 0; i < board.COL; i++) {
		players[0].create(Piece::Type::PAWN, glm::vec2(i, 1), board);
	}

	players[1].create(Piece::Type::ROOK, glm::vec2(0, 7), board);
	players[1].create(Piece::Type::KNIGHT, glm::vec2(1, 7), board);
	players[1].create(Piece::Type::BISHOP, glm::vec2(2, 7), board);
	players[1].create(Piece::Type::QUEEN, glm::vec2(3, 7), board);
	players[1].create(Piece::Type::KING, glm::vec2(4, 7), board);
	players[1].create(Piece::Type::BISHOP, glm::vec2(5, 7), board);
	players[1].create(Piece::Type::KNIGHT, glm::vec2(6, 7), board);
	players[1].create(Piece::Type::ROOK, glm::vec2(7, 7), board);
	for (int i = 0; i < board.COL; i++) {
		players[1].create(Piece::Type::PAWN, glm::vec2(i, 6), board);
	}
}