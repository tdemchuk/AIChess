
#include "AI.h"

//Heuristic Evaluation function is at line 126

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

				if (score.x > curBeta) {
					return score;
				}

				if (score.x > curAlpha) {
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

				
				if (score.x < curAlpha) {
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


/* This function evalutes the heuristic value of a provided board state using 3 heuristic evaluation strategies:

   [A] Placing a player in check applies + or - 450 points to the heuristic total, depending if that player is white or black
	   respectively. This amount is half the value of the king and heavily encourages plays that will apply pressure to the
	   opposition, as well as prevent making moves that leave the AI's king vulnerable.

   [B] Each piece has its own value assigned to it, 10 for pawn, 30 for knight and bishop, 50 for rook, 90 for queen, and 900 for king.
	   The values are added to the total heuristic value if they are black pieces and subtracted if they are white. A basic strategy that
	   encourages capturing pieces and prioritizing pieces that are of higher value, with the Kings being the most valuable to capture or lose.
	   Heavily encourages defensive and offensive moves, while preventing over aggression (i.e. will not encourage sacrificing a rook for a pawn)

   [C] The relative rank of each piece is averaged and weighed against the opponent's average rank. If the black team's relative rank is higher, it
	   is added to the total, if the white team's relative rank is higher it subtracts from the total. The heuristic is subtle and encourages overall 
	   aggressive piece progression during stale conditions. Overall, it encourages the AI to have central control, trap the opponent's king, and promote
	   pawns as fast as possible.

*/
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

	//Apply for each player to be in Check [A]
	if (board.isThreatened(*(kings[0]))) heurValue += 450;
	if (board.isThreatened(*(kings[1]))) heurValue -= 450;

	//Apply piece values to heuristic [B]
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

	//Apply Relative Rank Heuristic [C]

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
