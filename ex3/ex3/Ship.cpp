#include "Ship.h"
#include <algorithm>    // std::sort 
#include <iostream>

Ship::Ship(std::pair<char, std::set<Coordinate>> input)
{
	symbol = toupper(input.first);

	if (symbol == RUBBER_BOAT) {
		setFields(1, 2, input.second);
	}
	else if (symbol == ROCKET_SHIP) {
		setFields(2, 3, input.second);
	}
	else if (symbol == SUBMARINE) {
		setFields(3, 7, input.second);
	}
	else if (symbol == DESTROYER) {
		setFields(4, 8, input.second);
	}
	else
	{
		setFields(-1, -1, input.second);
	}
}

int Ship::getPoints()const
{
	return points;
}

int Ship::getNotHit()const
{
	return notHit;
}

bool Ship::isAlive()const
{
	if (notHit > 0) {
		return true;
	}
	return false;
}

std::vector<Coordinate> Ship::getCoordinates()	
{
	std::vector <Coordinate> coors;
	for (auto& part : body)
	{
		coors.push_back(part.first);
	}
	return coors;
}


bool Ship::isValidShipLen(char id, size_t setSize)
{
	char letter = toupper(id);
	if (letter == RUBBER_BOAT) {
		if (setSize != RUBBER_BOAT_LEN) {
			return false;
		}
	}
	else if (letter == ROCKET_SHIP) {
		if (setSize != ROCKET_SHIP_LEN) {
			return false;
		}
	}
	else if (letter == SUBMARINE) {
		if (setSize != SUBMARINE_LEN) {
			return false;
		}
	}
	else if (letter == DESTROYER) {
		if (setSize != DESTROYER_LEN) {
			return false;
		}
	}
	else { // in any other case 
		return false;
	}

	return true;
}


bool Ship::isValidShipCoordinates(const std::set<Coordinate>& coordinates)
{
	// ship must contain at least 1 coordinate to exist
	if (coordinates.empty()) return false;

	std::vector<int> rows;
	std::vector<int> cols;
	std::vector<int> depths;

	// extract all rows && columns coordinates
	for (auto& coor : coordinates) {
		rows.push_back(coor.row);
		cols.push_back(coor.col);
		depths.push_back(coor.depth);
	}

	// sort the vecors
	std::sort(rows.begin(), rows.end());
	std::sort(cols.begin(), cols.end());
	std::sort(depths.begin(), depths.end());

	//check if ship is Horizontal
	if (isConstantCoors(rows) && isConstantCoors(depths) && isIncrementalCoors(cols)){
		return true;
	}

	//check if ship is vertical
	if (isConstantCoors(cols) && isConstantCoors(depths) && isIncrementalCoors(rows)) {
		return true;
	}
	//check if ship is dimantional
	if (isConstantCoors(rows) && isConstantCoors(cols) && isIncrementalCoors(depths)) {
		return true;
	}

	return false;
}



int Ship::updateAttack(int row, int col,int depth)
{
	for (auto& coor : body) {
		// if the coordinates attackd belong to me 
		if ((coor.first.row == row) && (coor.first.col == col) && (coor.first.depth == depth)) {

			if (coor.second == 0) { // no hit yet
				coor.second = 1; // update body
				notHit -= 1;
				return 0; // succesful attack
			}

			else { // my coordinates but already hit before
				return 1;
			}
		}
	}
	return -1; // not my coordinates
}


bool Ship::isValidShipDetails(std::pair<char, std::set<Coordinate>> input)
{
	if (!isValidShipLen(input.first, input.second.size())) {
		return false;
	}
	if (!isValidShipCoordinates(input.second)) {
		return false;
	}
	return true;
}


void Ship::setFields(int length, int sPoints, std::set<Coordinate> coordinates)
{
	Coordinate coorToInsert(0, 0, 0);
	len = length;
	points = sPoints;
	notHit = length;
	for (auto coor : coordinates) {
		coorToInsert.row = coor.row;
		coorToInsert.col = coor.col;
		coorToInsert.depth = coor.depth;
		body.insert(std::make_pair(coorToInsert, 0));
	}
}

bool Ship::isConstantCoors(const std::vector<int>& coors)
{
	if (coors.empty()) return false;
	
	int firstCoor;
	// compare all coordinates to the first coordinate
	firstCoor = coors[0];
	for (size_t i = 1; i < coors.size(); i++) {
		if (coors[i] != firstCoor) {
			return false;
		}
	}
	return true;
}


bool Ship::isIncrementalCoors(const std::vector<int>& coors)
{
	
	if (coors.empty())  return false;
	
	//check if the coordinates are incremental 
	auto prevCoor = coors[0];
	for (auto i = 1; i < coors.size(); i++) {
		if (coors[i] != (prevCoor + 1)) return false;
		prevCoor += 1;
	}
	return true;
}


std::list<Ship> Ship::createShipsList(const std::set<std::pair<char, std::set<Coordinate>>>& coordsSet)
{
	std::list<Ship> ships;
	
	for (auto& elem : coordsSet) {
		ships.emplace_back(Ship(elem));		
	}

	return ships;
} 