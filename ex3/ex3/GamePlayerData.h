#pragma once

#include "IBattleshipGameAlgo.h"
#include "ShipsBoard.h"
#include <memory>

/**
 * \brief keeps all algo details for specific single game
 */
class GamePlayerData {
	
	friend class BattleshipGameManager;

private:

	int id;											/* player id in single game - 0/1*/
	IBattleshipGameAlgo* playerAlgo;				/* pointer to the player algo. the responsobility is in battleshipGameManager to free this pointer, not in this class*/
	ShipsBoard shipsBoard;							
	size_t currShipsCount;							/* current avaliable ship for the player*/
	int score;										/* current score */
	bool hasMoreMoves;

	static const int UNDEFINED_PLAYERID = -1;

	GamePlayerData() : id(UNDEFINED_PLAYERID), playerAlgo(nullptr), shipsBoard(), currShipsCount(0), score(0), hasMoreMoves(true) {}
	
	GamePlayerData(int playerID, IBattleshipGameAlgo* inputPlayerAlgo, ShipsBoard inputShipsBoard, size_t shipsCount) : id(playerID), playerAlgo(inputPlayerAlgo), shipsBoard(std::move(inputShipsBoard)), currShipsCount(shipsCount), score(0), hasMoreMoves(true) {}
	
	~GamePlayerData() = default;

	GamePlayerData& operator=(const GamePlayerData& otherPlayer) = delete;			/* deletes assignment operator */
	GamePlayerData(const GamePlayerData& otherPlayer) = delete;						/* deletes copy constructor */
	
	GamePlayerData(GamePlayerData&& other)noexcept = delete;						/* deletes move c'tor */
	GamePlayerData& GamePlayerData::operator=(GamePlayerData&& other) noexcept;		/*  move assignment */
	
	
	/*get next valid attack coordinates,if player doesnt have more moves return <-1,-1,-1>*/
	Coordinate getAlgoNextAttack() const;

	/* given coordinates player updates number of ships he has got left&& updates the attack in his ships details.
	return the attack result and the number of points the attack scored
	if player attacked the same coordinate in the second time return <Hit, -1> to indicate that the player shoudnt keep his turn
	*/
	std::pair<AttackResult, int> realAttack(Coordinate coor);

	/* update players score to the prev score + num
	assume- num >= 0
	*/
	void incrementScore(int value) { if (value > 0) score += value; }

	//bool isSet()const { return id != UNDEFINED_PLAYERID && playerAlgo && shipsBoard.isSet() && currShipsCount > 0; }

};