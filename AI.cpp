
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

	//Base case: depth = ply
	if (depth == m_Ply) {
		score.x = EvalHeuristic(board, players, kings);
		return score;
	}

	//Generate available moves for the current player
	std::vector<Gameboard::Playable> playables = board.genPlayables(players[curPlayer].getOwned(), *(kings[curPlayer]));

	//Iterate through each move
	for (int i = 0; i < playables.size(); i++) {

		for (int j = 0; j < playables[i].moves.size(); j++) {

			//Make move and push to history stack [so it can be undone]
			board.move(playables[i].piece, playables[i].moves[j]);

			score = ABPrune(board, players, kings, curAlpha, curBeta, 1 - curPlayer, depth + 1);
			

			if (score.x == localbestScore.x) {
				if (rand() % 2 == 1) { //Randomly decides on moves that are equally as good

					bestMove.x = i;
					bestMove.y = j;

				}
			}
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

int AI::EvalHeuristic(Gameboard& board, Player(&players)[2], Piece* (&kings)[2]) {

	int heurValue = 0;
	double whiteAvg = 0;
	double blackAvg = 0;
	int whiteNC = 0; //white pieces not captured
	int blackNC = 0; //black pieces not captured
	std::vector<Piece*> whiteOwned = players[0].getOwned();
	std::vector<Piece*> blackOwned = players[1].getOwned();

	//0 is human player
	//1 is AI player

	//Apply for each player to be in Check
	if (board.isThreatened(*(kings[0]))) heurValue += 450;
	if (board.isThreatened(*(kings[1]))) heurValue -= 450;

	//Apply piece values to heuristic
	for (int i = 0; i < whiteOwned.size(); i++) {

		if (blackOwned[i]->getStatus() != Piece::Status::CAPTURED) {

			blackAvg += blackOwned[i]->relativeRank();
			blackNC += 1;

			switch (blackOwned[i]->type()) {

				case Piece::Type::KING: heurValue += 900;
					break;
				case Piece::Type::QUEEN: heurValue += 90;
					break;
				case Piece::Type::ROOK: heurValue += 50;
					break;
				case Piece::Type::BISHOP: heurValue += 30;
					break;
				case Piece::Type::KNIGHT: heurValue += 30;
					break;
				case Piece::Type::PAWN: heurValue += 10;

			}

		}

		if (whiteOwned[i]->getStatus() != Piece::Status::CAPTURED) {

			whiteAvg += whiteOwned[i]->relativeRank();
			whiteNC += 1;

			switch (whiteOwned[i]->type()) {

				case Piece::Type::KING: heurValue -= 900;
					break;
				case Piece::Type::QUEEN: heurValue -= 90;
					break;
				case Piece::Type::ROOK: heurValue -= 50;
					break;
				case Piece::Type::BISHOP: heurValue -= 30;
					break;
				case Piece::Type::KNIGHT: heurValue -= 30;
					break;
				case Piece::Type::PAWN: heurValue -= 10;

			}

		}

	}

	//Apply Relative Rank Heuristic

	blackAvg /= blackNC;
	whiteAvg /= whiteNC;

	if (blackAvg > whiteAvg) {
		heurValue += blackAvg;
	}
	else {
		heurValue -= whiteAvg;
	}

	return heurValue;
	

}
