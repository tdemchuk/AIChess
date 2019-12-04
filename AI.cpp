
#include "AI.h"
#include <iostream>

AI::AI(int ply)
	: m_Ply(ply)
{
}

const int AI::Ply() const
{
	return m_Ply;
}

glm::vec2 AI::ABPrune(Gameboard& board, Player& players, Piece* kings, int curPlayer, int depth)
{
	glm::vec2 score;
	score.x = 0;
	glm::vec2 bestScore; //The best score achieved for the possible moves, stored in the x value
	bestScore.x = 0;
	glm::vec2 bestMove;	 //The best move [piece,move] that corresponds to the best score
	int minmax = (depth % 2) == 0 ? 1 : -1; //Whether to min or max

	//TODO -- Check if swap player is needed? (Ref line 19 of code)

	//Base case: depth = ply
	if (depth == m_Ply) {
	
		score.x = EvalHeurisitc(board);
		return score;
	
	}

	//Generate available moves for the current player
	std::vector<Gameboard::Playable> playables = board.genPlayables(players[curPlayer].getOwned(), kings[curPlayer]);



	return score;
}

int EvalHeurisitc(Gameboard& board) {

	return 1;

}
