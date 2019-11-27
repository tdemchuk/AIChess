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

	const Color team() const;											// Return the team this player is on
	std::string teamStr() const;										// Returns a string decsribing which team this player is on [color string]
	Piece* create(Piece::Type type, glm::vec2 pos, Gameboard& board);	// Create a piece as owned by this Player and place on the given gameboard
	std::vector<Piece*>& getOwned();									// Returns reference to the list of pieces this player owns
	void capture(Piece* piece);											// Adds a piece to the list of pieces this player has captured
	std::vector<Piece*>& getCaptured();									// Returns reference to the list of pieces this player has captured

private:
	const Color m_team;
	std::vector<Piece*> m_owned;
	std::vector<Piece*> m_captured;
};

#endif