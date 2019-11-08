#ifndef PLAYER_H
#define PLAYER_H

#include "Color.h"
#include "Piece.h"
#include "Gameboard.h"
#include "GameState.h"
#include <vector>


/*
	Player Class
*/
class Player
{
public:

	Player() = delete;
	Player(Color team);
	~Player();

	const Color team() const;										// Return the team this player is on
	void create(Piece::Type type, glm::vec2 pos, Gameboard& board);	// Create a piece as owned by this Player and place on the given gameboard
	const std::vector<Piece*>& getOwned() const;					// Returns reference to the list of pieces this player owns
	void capture(Piece* piece);										// Adds a piece to the list of pieces this player has captured
	const std::vector<Piece*>& getCaptured() const;					// Returns reference to the list of pieces this player has captured
	GameState takeTurn();											// Player takes turn

private:
	const Color m_team;
	std::vector<Piece*> m_owned;
	std::vector<Piece*> m_captured;
};

#endif