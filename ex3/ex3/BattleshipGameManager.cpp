#include "BattleshipGameManager.h"
#include "GamePlayerData.h"
#include "PlayerGameResultData.h"
#include "BoardDataImpl.h"

// pass unique_ptr by value as described here (pass responsibility) - https://stackoverflow.com/a/8114913
BattleshipGameManager::BattleshipGameManager(const BattleshipBoard & board, std::unique_ptr<IBattleshipGameAlgo> algoA, std::unique_ptr<IBattleshipGameAlgo> algoB) : mainBoard(board), algorithmA(std::move(algoA)), algorithmB(std::move(algoB))
{
	
	std::set<std::pair<char, std::set<Coordinate>>> shipDetailsA, shipDetailsB;
	mainBoard.ExtractShipsDetailsOfGamePlayers(shipDetailsA, shipDetailsB);
	
	ShipsBoard shipsBoardA, shipsBoardB;
	BoardDataImpl boardDataA(PLAYERID_A, mainBoard);
	
	initPlayerData(PLAYERID_A, algorithmA.get(), shipDetailsA, shipsBoardA, boardDataA);

	playerA = std::move(GamePlayerData(PLAYERID_A, algorithmA.get(), std::move(shipsBoardA), shipDetailsA.size()));
	
	BoardDataImpl boardDataB(PLAYERID_B, mainBoard);
	initPlayerData(PLAYERID_B, algorithmB.get(), shipDetailsB, shipsBoardB, boardDataB);
	
	playerB = std::move(GamePlayerData(PLAYERID_B, algorithmB.get(), std::move(shipsBoardB), shipDetailsB.size()));

}

void BattleshipGameManager::initPlayerData(int playerId, IBattleshipGameAlgo* playerAlgo, std::set<std::pair<char, std::set<Coordinate>>>& shipsDetails, ShipsBoard& playerShipBoard, const BoardDataImpl& playerBoardData)const
{	
	playerAlgo->setPlayer(playerId);
	playerAlgo->setBoard(playerBoardData);
	auto playerShipsList = Ship::createShipsList(shipsDetails);
	playerShipBoard = ShipsBoard(playerShipsList, mainBoard.getRows(), mainBoard.getCols(), mainBoard.getDepth());	
}

PlayerGameResultData BattleshipGameManager::Run()
{
	Coordinate nextAttack(-1,-1,-1);
	std::pair<AttackResult, int> attackRes;
	GamePlayerData* currPlayer = &playerA;
	GamePlayerData* otherPlayer = &playerB;

	// as long as one of the players has more moves and no one won
	while (currPlayer->hasMoreMoves || otherPlayer->hasMoreMoves) {

		if (!currPlayer->hasMoreMoves) {
			// if current player doesnt have anymore moves continue to next player
			std::swap(currPlayer, otherPlayer);
			continue;
		}
		nextAttack = currPlayer->getAlgoNextAttack();
		if (nextAttack.row == -1 && nextAttack.col == -1 && nextAttack.depth == -1) {
			// current player finished
			currPlayer->hasMoreMoves = false;
			continue;
		}
		// if got here the attack coordinates are in the board 

		// attack other player 
		attackRes = otherPlayer->realAttack(nextAttack);

		if (attackRes.first == AttackResult::Miss) {
			// the opponent doesnt have a ship in this coordinates; check if attacked myself
			attackRes = currPlayer->realAttack(nextAttack);

			if (attackRes.first != AttackResult::Miss) { // currPlayer attacked himself
														 // the other player gets points
				otherPlayer->incrementScore(attackRes.second);
			}
			currPlayer->playerAlgo->notifyOnAttackResult(currPlayer->id, nextAttack, attackRes.first);
			otherPlayer->playerAlgo->notifyOnAttackResult(currPlayer->id, nextAttack, attackRes.first);
			// pass turn to other player- if missed || if attacked myself
			std::swap(currPlayer, otherPlayer);
			//check if someone won
			if ((currPlayer->currShipsCount == 0) || (otherPlayer->currShipsCount == 0)) {
				break;
			}
		}
		else {
			if (attackRes.second == -1) {	// hit opponents ship but not in a new coordinate; switch turns
				std::swap(currPlayer, otherPlayer);
			}
			else {
				currPlayer->incrementScore(attackRes.second);
				currPlayer->playerAlgo->notifyOnAttackResult(currPlayer->id, nextAttack, attackRes.first);
				otherPlayer->playerAlgo->notifyOnAttackResult(currPlayer->id, nextAttack, attackRes.first);
				// keep my turn 
			}
			//check if someone won
			if ((currPlayer->currShipsCount == 0) || (otherPlayer->currShipsCount == 0)) { 
				break;
			}
		}
	}
	return outputGameResult(currPlayer, otherPlayer);

}


PlayerGameResultData BattleshipGameManager::outputGameResult(GamePlayerData* currPlayer, GamePlayerData* otherPlayer)
{
	int currScore = currPlayer->score;
	int otherScore = otherPlayer->score;

	if (currPlayer->currShipsCount == 0) {
		if (currPlayer->id == PLAYERID_A) { // currPlayer is playerA
			return PlayerGameResultData("", LOST, WON, currScore, otherScore);
		}
		else { // otherPlayer is playerA
			return PlayerGameResultData("", WON, LOST, otherScore, currScore);
		}
	}

	if (otherPlayer->currShipsCount == 0) {
		if (otherPlayer->id == PLAYERID_A) { // otherPlayer is playerA
			return PlayerGameResultData("", LOST, WON, otherScore, currScore);
		}
		else {// currPlayer is playerA
			return PlayerGameResultData("", WON, LOST, currScore, otherScore);
		}
	}

	//its a tie
	if (currPlayer->id == PLAYERID_A) {
	
		return PlayerGameResultData("", LOST, LOST, currScore, otherScore);
	}
	else {
		return PlayerGameResultData("", LOST, LOST, otherScore, currScore);
	}
	
}

