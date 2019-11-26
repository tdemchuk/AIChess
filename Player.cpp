
#include "Player.h"
#include <iostream>


Player::Player(Color team)
	: m_team(team)
{
}

Player::~Player()
{
	for (int i = 0; i < m_owned.size(); i++) {
		delete m_owned[i];	// It is the players responsibility to "clean up" their pieces
	}
}

const Color Player::team() const
{
	return m_team;
}

std::string Player::teamStr() const
{
	switch (m_team) {
	case BLACK: return "Black";
	case WHITE: return "White";
	}
	return "NO TEAM";
}

Piece* Player::create(Piece::Type type, glm::vec2 pos, Gameboard& board)
{
	if (!board.isValidCoord(pos)) throw std::out_of_range("Spawn Point is Not a Valid Board Coordinate");
	Piece* piece = new Piece(type, m_team);		// create the piece
	m_owned.push_back(piece);					// Add created piece to list of pieces this player owns
	Piece* overwritten = nullptr;				// If created piece overwrites a piece currently on the board, delete it
	overwritten = board.place(piece, pos);		// Place the piece on the board
	if (overwritten) std::cout << "A Piece was Overwritten!\n";
	delete overwritten;							// Clean up overwritten piece --> will likely cause errors throughoput program run
	return piece;
}

std::vector<Piece*>& Player::getOwned()
{
	return m_owned;
}

void Player::capture(Piece* piece)
{
	m_captured.push_back(piece);
}

std::vector<Piece*>& Player::getCaptured()
{
	return m_captured;
}