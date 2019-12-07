
#include "AI.h"

AI::AI(int ply)
	: m_Ply(ply)
{
}

const int AI::Ply() const
{
	return m_Ply;
}

glm::vec2 AI::ABPrune(Gameboard& board, Player (&players)[2], Piece* (&kings)[2], int curAlpha, int curBeta, int curPlayer, int depth)
{
	glm::vec2 score;
	score.x = 0;
	glm::vec2 localbestScore; //The best score achieved for the possible moves, stored in the x value
	glm::vec2 bestMove;	 //The best move [piece,move] that corresponds to the best score
	int minmax = (depth % 2) == 0 ? 1 : -1; //Whether to min or max
	if (minmax == 1) {
		localbestScore.x = INT_MIN;
	}
	else {
		localbestScore.x = INT_MAX;
	}
	bool inCheck = false;

	//Base case: depth = ply
	if (depth == m_Ply) {
		score.x = EvalHeuristic(board);
		return score;
	}

	//Generate available moves for the current player
	std::vector<Gameboard::Playable> playables = board.genPlayables(players[curPlayer].getOwned(), *(kings[curPlayer]));

	//Test for current player to be in Check
	inCheck = board.isThreatened(*(kings[curPlayer]));

	//Immediately return end board states
	if (playables.size() == 0) {
		if (inCheck) {
			if (curPlayer) score.x = INT_MAX; //Curplayer 1 is human, 0 moves available means AI wins
			else score.x = INT_MIN; //Curplayer 0 is AI, 0 moves available means human wins
		}
		else {
			score.x = 0; //Draws are neutral and do not impact decisions
		}

		return score;
	}

	//Iterate through each move
	for (int i = 0; i < playables.size(); i++) {

		for (int j = 0; j < playables[i].moves.size(); j++) {

			//Make move and push to history stack [so it can be undone]
			board.move(playables[i].piece, playables[i].moves[j]);

			//Tests for returning the proper value according to minmax (proves ply 1 and 2 work)

			score = ABPrune(board, players, kings, curAlpha, curBeta, 1 - curPlayer, depth + 1);
			

			/*if (score.x == localbestScore.x) {
				if (rand() % playables.size() == 1) { //Randomly decides on moves that are equally as good

					bestMove.x = i;
					bestMove.y = j;

				}
			}*/
			if (minmax == 1) { //Maximize Scores
				if (score.x > localbestScore.x) {
					localbestScore.x = score.x;
					bestMove.x = i;
					bestMove.y = j;
					
				}
				//Undo move
				board.undo();

				//Prune
				
				if (score.x >= curBeta) {
					return score;
				}
				else if (score.x > curAlpha) {
					curAlpha = score.x;
				}
				
			}
			else { //Minimize Score
				if (score.x < localbestScore.x) {
					localbestScore.x = score.x;
					bestMove.x = i;
					bestMove.y = j;
				}
				//Undo move
				board.undo();

				//Prune
				
				if (score.x <= curAlpha) {
					return score;
				}
				if (score.x < curBeta) {
					curBeta = score.x;
				}
			}

		}
	}

	if (depth == 0) return bestMove;
	else return localbestScore;
}

int AI::EvalHeuristic(Gameboard& board) {

	return 10;

}
