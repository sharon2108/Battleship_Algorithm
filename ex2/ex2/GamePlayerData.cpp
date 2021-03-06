#include "GamePlayerData.h"
#include <iostream>
#include "BattleshipBoard.h"
#include "BattleshipGameManager.h"


GamePlayerData & GamePlayerData::operator=(GamePlayerData && other) noexcept
{
	
	id = other.id;
	std::swap(playerAlgo, other.playerAlgo);
	hasMoreMoves = other.hasMoreMoves;
	score = other.score;
	std::swap(shipsMatrix, other.shipsMatrix);
	currShipsCount = other.currShipsCount;
	boardRows = other.boardRows;
	boardCols = other.boardCols;

	
	return *this;
}

GamePlayerData::~GamePlayerData()
{
	Ship::deleteShipMatrix(shipsMatrix , boardRows, boardCols);

}

std::pair<int, int> GamePlayerData::getAlgoNextAttack() const
{

	std::pair<int, int> tmpAttack = playerAlgo->attack();

	
	if (tmpAttack.first == -1 && tmpAttack.second == -1) { // player doesnt have anymore moves 
		return tmpAttack;
	}

	// while the given coordinates are not in the board && the player has more moves
	while (!BattleshipBoard::isCoordianteInBoard(tmpAttack.first - 1, tmpAttack.second - 1, boardRows, boardCols))
	{
		tmpAttack = playerAlgo->attack();
		if (tmpAttack.first == -1 && tmpAttack.second == -1) { // player doesnt have anymore moves
			return tmpAttack;
		}

	}
	return tmpAttack;
}


std::pair<AttackResult, int> GamePlayerData::realAttack(std::pair<int, int> coor)
{
	if (!BattleshipBoard::isCoordianteInBoard(coor.first - 1, coor.second - 1, boardRows, boardCols)){
		return std::pair<AttackResult, int>(AttackResult::Miss, 0);
	}

	int attackRes;
	Ship* shipPtr = shipsMatrix[coor.first - 1][coor.second - 1];
	std::pair<AttackResult, int> retPair;

	if (shipPtr == nullptr) {// doesnt have a ship in this coordinates
		retPair = std::pair<AttackResult, int>(AttackResult::Miss, 0);
	}
	else { // have a ship in this coordinates 
		if (shipPtr->isAlive()) {// not sank yet
			attackRes = shipPtr->updateAttack(coor.first - 1, coor.second - 1);
			if (attackRes == 0) { // successful attack
				if (shipPtr->isAlive()) { // not sank yet 
					retPair = std::make_pair(AttackResult::Hit, 0);
				}
				else
				{ // the ship sank after the last attack
					retPair = std::make_pair(AttackResult::Sink, shipPtr->getPoints());
				}
			}
			else if (attackRes == 1)
			{ // already hit this part- no points given
				retPair = std::pair<AttackResult, int>(AttackResult::Hit, -1);
			}
			else
			{ // attackRes == -1  
				retPair = std::pair<AttackResult, int>(AttackResult::Miss, 0);
			}
		}
		else { // this ship is mine but already sank
			retPair = std::pair<AttackResult, int>(AttackResult::Miss, 0);
		}
	}

	if (retPair.first == AttackResult::Sink) { //  update number ships left for player
		currShipsCount = currShipsCount - 1;
	}

	return retPair;
}