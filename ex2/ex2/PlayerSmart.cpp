#include "PlayerSmart.h"
#include <algorithm>



std::pair<int, int> PlayerSmart::attack()
{

	if (size(shipsInProcess) == 0) // no ships in process 
	{
		// return random coordinate
		return BattleshipGameUtils::randomElement(attackOptions.begin(), attackOptions.end());
	}
	// already have ships in shipsInProcess
	
	return nextAttackFromCoors(shipsInProcess[0], size(shipsInProcess[0]));
}


void PlayerSmart::removeAllIrreleventCoordinates(const std::pair<int, int>& pair)
{
	std::pair<int, int> removeCandidate;
	if(isVertical)
	{
		updateCoordinates(removeCandidate, pair.first, pair.second - 1);
		removeOneCoordinate(removeCandidate);
		updateCoordinates(removeCandidate, pair.first, pair.second + 1);
		removeOneCoordinate(removeCandidate);
	}

	if (isHorizontal)
	{
		updateCoordinates(removeCandidate, pair.first -1, pair.second);
		removeOneCoordinate(removeCandidate);
		updateCoordinates(removeCandidate, pair.first +1, pair.second);
		removeOneCoordinate(removeCandidate);
		
	}
}


void PlayerSmart::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	int mergeResult;
	std::pair<int, int> nextPairTosearch;
	std::pair <int,int> attackedPair(row, col) ;
	isVertical = false;
	isHorizontal = false;

	if (attackedPair is coordinate of my ship)
	{
		removeOneCoordinate(attackedPair);
	}

	if (result == AttackResult::Hit)
	{
		mergeResult = addCoorToShipsInProcess(row,col , &nextPairTosearch,-1);
		if (mergeResult != -1)// the coordinate is added to one of the ships that are already in process 
		{ // try to chekc if there is another shipdetails in process that belong to the same ship i added the recent attacked pair.
			mergeShipDetails(&nextPairTosearch, mergeResult);			
			removeAllIrreleventCoordinates(attackedPair);
		}

		else{
			removeOneCoordinate(attackedPair);
		}
	}

	else if (result == AttackResult::Sink)
	{
		//if (size 1 ) // notify doesnt tell what size the ship was
		//{
		//	removeAllIrreleventCoordinates
		//}

		// find to whom it belongs and delete the set+ envi
		mergeResult = addCoorToShipsInProcess(row, col, &nextPairTosearch, 1);
		if (mergeResult == -1) // the ship wasnt in process >> ship of size 1  
		{
			removeAllIrreleventCoordinates(attackedPair);
			removeOneCoordinate(attackedPair);
		}
		else
		{
			removeAllIrreleventCoordinates(attackedPair);// will remove up/down or left/right to attacked pair 
			removeDetails(attackedPair, mergeResult);
			removeOneCoordinate(attackedPair);

		}

	}
	else // result == AttackResult::Miss
	{
		removeOneCoordinate(attackedPair);
	}

	implemenent comparator!!!  
	//sort the vector by size 
	std::sort(shipsInProcess.begin(), shipsInProcess.end());
}


bool PlayerSmart::shipInProcess::isVertical()
{
}

void PlayerSmart::removeDetails(std::pair<int, int>& nextPair, int merge_result)
{
	std::pair<int, int> removePair(-1, -1);
	std::vector<int> rows;
	std::vector<int> cols;
	
	// finds where the last coordinate is 
	decltype(shipsInProcess)::iterator shipsInprocessIter;
?	shipsInProcess.begin();
	// the iterator doesnt sta\rt from the cooerct place 
	for (int i = merge_result; i< size(shipsInProcess); i++)
	{
		auto it = shipsInProcess[i].find(nextPair);
		if (it != shipsInProcess[i].end())
		{
			shipsInProcess[merge_result].insert(nextPair);
			shipsInProcess.erase(shipsInprocessIter);
			break;
		}
		++shipsInprocessIter;
	}

	when i merge 2 sets need to remove vertical/horizontal for all the members
	updateDetailsAboutShip(shipsInProcess[merge_result],&rows, &cols);
	if (isHorizontal)
	{
		updateCoordinates(removePair, rows[0], cols[0] - 1);
		removeOneCoordinate(removePair);
		updateCoordinates(removePair, rows[0], cols[size(cols)-1] + 1);
		removeOneCoordinate(removePair);
	}

	else if (isVertical)
	{
		updateCoordinates(removePair, rows[0]-1, cols[0]);
		removeOneCoordinate(removePair);
		updateCoordinates(removePair, rows[size(rows)]+1, cols[0]);
		removeOneCoordinate(removePair);
	}



?	shipsInprocessIter->begin();
	//erase the ship from vector


}

void PlayerSmart::mergeShipDetails(std::pair<int, int>* pair , int indexToupdate)
{
	decltype(shipsInProcess)::iterator shipsInprocessIter;

	for(int i = indexToupdate; i< size(shipsInProcess);i++)
	{
		auto it = shipsInProcess[i].find(*pair);
		if (it != shipsInProcess[i].end())
		{
			shipsInProcess[indexToupdate].insert(*pair);
			shipsInProcess.erase(shipsInprocessIter);
			break;
		}
		++shipsInprocessIter;
	}

}


std::pair<int, int> PlayerSmart::nextAttackFromCoors(const std::set<std::pair<int, int>>& hitCoordinates, int numOfCoors)
{
	std::pair<int, int> attackCandidate(-1,-1);
	std::vector<int> rows;
	std::vector<int> cols;
	isHorizontal = false;
	isVertical = false;

	if (numOfCoors < 1) { 
		// shouldnt get here 
		return attackCandidate;
	}

	/*rows; cols;isVertical = false;isHorizontal = false;*/
	updateDetailsAboutShip(hitCoordinates, &rows, &cols);

	if (isHorizontal) {
		updateCoordinates(attackCandidate, rows[0], cols[0] - 1);
		if (isInAttackOptions(attackCandidate)) //check left
		{
			return attackCandidate;
		}

		updateCoordinates(attackCandidate, rows[0], cols[numOfCoors - 1] + 1);
		if (isInAttackOptions(attackCandidate)){ // check right
			return attackCandidate;
		}
	}

	if (isVertical){
		updateCoordinates(attackCandidate, rows[0] - 1, cols[0]); // check down 
		if (isInAttackOptions(attackCandidate)) 
		{
			return attackCandidate;
		}
		updateCoordinates(attackCandidate, rows[numOfCoors-1] + 1, cols[0]); // check up
		if (isInAttackOptions(attackCandidate)) { // highest posible coor
			return attackCandidate;
		}
	}
	// shouldnt get here
	updateCoordinates(attackCandidate,-1, - 1);
	return attackCandidate;
}

void PlayerSmart::updateDetailsAboutShip(const std::set<std::pair<int, int>>&allCoors,std::vector<int>* rowCoors, std::vector<int>* colCoors)
{
	int shipsSize = size(allCoors);
	// extract all rows && columns coordinates
	for (auto& coor : allCoors) {
		rowCoors->push_back(coor.first);
		colCoors->push_back(coor.second);
	}
	// sort the vectors
	std::sort(rowCoors->begin(), rowCoors->end());
	std::sort(colCoors->begin(), colCoors->end());

	//check if ship is Horizontal
	if (isConstantCoors(*rowCoors, shipsSize)) {
		if (isIncrementalCoors(*colCoors, shipsSize)) {
			isHorizontal = true;
		}
	}

	//check if ship is vertical
	if (isConstantCoors(*colCoors, shipsSize)) {
		if (isIncrementalCoors(*rowCoors, shipsSize)) {
			isVertical = true;
		}
	}
}


bool PlayerSmart::isInAttackOptions(const std::pair<int, int>& coors) const
{
	auto it = attackOptions.find(coors);
	if (it != attackOptions.end()) 
	{
		return true;
	}

	return false;
}

bool PlayerSmart::isConstantCoors(const std::vector<int>& coors, int size)
{
	int firstCoor;
	if (size < 1) { // invlid number of coordinates
		return false;
	}
	// compare all cordinated to the first coordinate
	firstCoor = coors[0];
	for (int i = 1; i < size; i++) {
		if (coors[i] != firstCoor) {
			return false;
		}
	}
	return true;
}

bool PlayerSmart::isIncrementalCoors(const std::vector<int>& coors, int size)
{
	int prevCoor;
	if (size < 1) { // invalid number of cordinates
		return false;
	}

	//check if the coordinates are incremental 
	prevCoor = coors[0];
	for (auto i = 1; i < size; i++) {
		if (coors[i] != (prevCoor + 1)) {
			return false;
		}
		prevCoor += 1;
	}
	return true;
}

/*check if the rows, and colsrepresent a legal ship if so , check from which side the coordinates of the ships 
 * are forming a sequence. and update  nextPair to hold the other side of the attacked coordinate
 * and return true, otherwise return false
 */
bool PlayerSmart::produceNextPair(std::vector<int>& rows, std::vector<int>& cols,
	int currRow , int currCol,std::pair<int, int>* nextPair) const
{
		int shipSize = size(rows);

		// ship must contain at least 1 coordinate to exist
		if (shipSize< 1) {
			return false;
		}

		//check if ship is Horizontal
		if (isHorizontal) {
			if (cols[0]== currCol) // the new coordinate added on the left of the ship
			{
				updateCoordinates(*nextPair, currRow, currCol - 1);
				//nextPair->first = currRow;
				//nextPair->second = currCol - 1;
			}
			else{
				updateCoordinates(*nextPair, currRow, cols[shipSize - 1] + 1);
				//nextPair->first = currRow;
				//nextPair->second = cols[shipSize-1] +1 ; // the new coordinate is from the right side of the vctor
			}
			return true;
		}

		//check if ship is vertical
		if (isVertical) {
			if (rows[0] == currRow) // the new coordinate is from the top of the ship
			{
				updateCoordinates(*nextPair, currRow - 1, currCol);
				//nextPair->first = currRow - 1;
				//nextPair->second = currCol;

			}
			else
			{
				updateCoordinates(*nextPair, rows[shipSize - 1] + 1, currCol);
				//nextPair->first = rows[shipSize - 1] + 1;  // the new coordinate is from the bottom
				//nextPair->second = currCol;
			}

			return true;
		}
	return false;

}


void PlayerSmart::removeOneCoordinate(std::pair<int,int>& pairToDelete)
{
	auto it = attackOptions.find(pairToDelete);
	if (it != attackOptions.end())
	{
		attackOptions.erase(it);
	}
}

/*
 * check if <row,col> could be considerd as part of existing process, if so add it and return the index it was add to.
 * otherwise create a new set for this ship and return -1
 */
int PlayerSmart::addCoorToShipsInProcess(int row, int col , std::pair<int, int>* nextPairTosearch , int sink )
{
	int i = 0;
	std::set < std::pair<int, int>> newSet;
	std::pair<int, int> currPair(row, col);
	std::vector<int> rowCoors;
	std::vector<int> colCoors;
	bool ret;


	for (auto details: shipsInProcess)
	{
		isVertical = false;
		isHorizontal = false;
		details.insert(currPair);
		updateDetailsAboutShip(details, &rowCoors, &colCoors);
		ret= produceNextPair(rowCoors, colCoors, row, col, nextPairTosearch);

		if (ret)
		{
			shipsInProcess[i].insert(currPair);
			return i;
		}
		i++;
		rowCoors.clear();
		colCoors.clear();
	}

	if (sink != 1)
	{
		newSet.insert(currPair);
		shipsInProcess.push_back(newSet);
	}

	return -1;
}



