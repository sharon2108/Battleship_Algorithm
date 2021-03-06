#include "BattleshipBoard.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include "BattleshipGameUtils.h"
#include <algorithm>

BattleshipBoard::BattleshipBoard(std::vector<char> board, int inputRows, int inputCols, int inputDepth) : boardVec(board), rows(inputRows), cols(inputCols), depth(inputDepth), isSuccCreated(true) {}

BattleshipBoard::BattleshipBoard(const std::string & boardPath) : isSuccCreated(false)
{
	if (boardPath.empty()) return;

	std::string line;

	std::ifstream boardFile(boardPath.c_str());
	if (boardFile.is_open())
	{
		/* parsing the first line to get the board dimensions */
		std::getline(boardFile, line);

		if (!parseBoardDimensions(line)) return;
		boardVec = std::move(InitNewEmptyBoardVector(rows, cols, depth));

		/* now parse the board - seperate matrix for every depth */
		for (int d = 0; d < depth; d++)
		{
			while (!BattleshipGameUtils::emptyLine(line) && std::getline(boardFile, line));
			/* skip intermediate line between every matrix, and skip redundant lines */

			int currRow = 0;
			while (currRow < rows && std::getline(boardFile, line) && !BattleshipGameUtils::emptyLine(line))
			{
				/* while we read < rows lines and we don't reach to eof in board file and line is not empty (end of current matrix) */
				CopyInputLineToBoard(line, d, currRow);
				currRow++;
			}
		}
		boardFile.close();
	}
	else return;				/* we can't open the board file */
	isSuccCreated = true;
}

BattleshipBoard::BattleshipBoard(const BoardData & boardData) : rows(boardData.rows()), cols(boardData.cols()), depth(boardData.depth()), isSuccCreated(true)
{
	boardVec = std::move(InitNewEmptyBoardVector(rows, cols, depth));
	
	for (int d = 1; d <= depth; d++)
		for (int r = 1; r <= rows; r++)
			for (int c = 1; c <= cols; c++)
				setCoord(r-1, c-1, d-1, boardData.charAt(Coordinate(r, c, d)));
}


BattleshipBoard::BattleshipBoard(BattleshipBoard && otherBoard) noexcept : boardVec(std::move(otherBoard.boardVec)), rows(otherBoard.rows), cols(otherBoard.cols), depth(otherBoard.depth), isSuccCreated(otherBoard.isSuccessfullyCreated()) {}

BattleshipBoard & BattleshipBoard::operator=(BattleshipBoard && otherBoard) noexcept
{
	boardVec = std::move(otherBoard.boardVec);
	rows = otherBoard.rows;
	cols = otherBoard.cols;
	depth = otherBoard.depth;
	isSuccCreated = otherBoard.isSuccCreated;
	return *this;
}


/* for battleship pixel in the board, we will check if the adjacent pixels of this pixel (up, down, left and right)
is a different ship - if so, we have an adjacent error in board.
*/
bool BattleshipBoard::CheckIfHasAdjacentShips() const
{
	char currPos;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < depth; k++)
			{
				currPos = (*this)(i, j, k);

				if (currPos == BLANK_CHAR) continue;			/* if current position is not a ship - it is a ' ', because we clean the board at the begining */

				auto nearbyCoordSet = getNearbyCoordinates({ i,j,k });

				for (auto adjacentCoor : nearbyCoordSet)		/* for each nearby coordinate we check if we have adjacent ship (surronding other char)  */
				{
					char adjacentVal = (*this)(adjacentCoor.row, adjacentCoor.col, adjacentCoor.depth);

					if (IsShipCharInBoard(adjacentVal) && adjacentVal != currPos)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool BattleshipBoard::IsShipCharInBoard(char ch)
{
	ch = toupper(ch);
	return (ch == RUBBER_BOAT || ch == ROCKET_SHIP || ch == SUBMARINE || ch == DESTROYER);
}

std::set<std::pair<char, std::set<Coordinate>>> BattleshipBoard::ExtractShipsDetails() const
{
	auto boardVecCopy = boardVec;				/* takes a copy of the boardVec, because we want to edit and delete found ships */
	std::set<std::pair<char, std::set<Coordinate>>> setOfShipsDetails;
	char currShipChar;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < depth; k++)
			{
				int currIndex = BattleshipGameUtils::calcCoordIndex(i, j, k, rows, cols);

				if ((currShipChar = boardVecCopy.at(currIndex)) == ' ') continue;

				std::set<Coordinate> coordOfCurrentShip;

				getAllCurrShipCoords(boardVecCopy, i, j, k, currShipChar, coordOfCurrentShip, rows, cols, depth);

				setOfShipsDetails.insert(std::make_pair(currShipChar, coordOfCurrentShip));				/* insert to the big set current ship details - <'m', {<1,2>,<1,3>}>*/

				for (auto coord : coordOfCurrentShip)
				{
					currIndex = BattleshipGameUtils::calcCoordIndex(coord.row, coord.col, coord.depth, rows, cols);
					boardVecCopy.at(currIndex) = ' ';													/* clear the board from this ship (we should have a copy of the board) */
				}
				coordOfCurrentShip.clear();
			}
		}
	}

	return std::set<std::pair<char, std::set<Coordinate>>>(setOfShipsDetails);
}

void BattleshipBoard::ExtractShipsDetailsOfGamePlayers(std::set<std::pair<char, std::set<Coordinate>>>& shipDetailsA, std::set<std::pair<char, std::set<Coordinate>>>& shipDetailsB)const
{
	auto allShipDeatils = ExtractShipsDetails();

	for (auto shipDetail : allShipDeatils)
	{
		if (isPlayerShip(PLAYERID_A, shipDetail.first))
			shipDetailsA.insert(shipDetail);
		else 
			shipDetailsB.insert(shipDetail);
	}

}

void BattleshipBoard::getAllCurrShipCoords(std::vector<char> board, int r, int c, int d, char currShipChar, std::set<Coordinate>& coordOfCurrentShip, int boardRows, int boardCols, int boardDepth)
{
	
	int CoordIndex = BattleshipGameUtils::calcCoordIndex(r, c, d, boardRows, boardCols);
	if (currShipChar == board.at(CoordIndex))
	{
		board.at(CoordIndex) = BLANK_CHAR;												/* clear the current position and add it to the coordinates set*/
		coordOfCurrentShip.insert({ r, c, d });

		if (BattleshipGameUtils::isCoordianteInBoard(r, c + 1, d, boardRows, boardCols, boardDepth))
			getAllCurrShipCoords(board, r, c + 1, d, currShipChar, coordOfCurrentShip, boardCols, boardRows, boardDepth);
		if (BattleshipGameUtils::isCoordianteInBoard(r, c - 1, d, boardCols, boardRows, boardDepth))
			getAllCurrShipCoords(board, r, c - 1, d, currShipChar, coordOfCurrentShip, boardCols, boardRows, boardDepth);
		if (BattleshipGameUtils::isCoordianteInBoard(r + 1, c, d, boardCols, boardRows, boardDepth))
			getAllCurrShipCoords(board, r + 1, c, d, currShipChar, coordOfCurrentShip, boardCols, boardRows, boardDepth);
		if (BattleshipGameUtils::isCoordianteInBoard(r - 1, c, d, boardCols, boardRows, boardDepth))
			getAllCurrShipCoords(board, r - 1, c, d, currShipChar, coordOfCurrentShip, boardCols, boardRows, boardDepth);
		if (BattleshipGameUtils::isCoordianteInBoard(r, c, d + 1, boardCols, boardRows, boardDepth))
			getAllCurrShipCoords(board, r, c, d + 1, currShipChar, coordOfCurrentShip, boardCols, boardRows, boardDepth);
		if (BattleshipGameUtils::isCoordianteInBoard(r, c, d - 1, boardCols, boardRows, boardDepth))
			getAllCurrShipCoords(board, r, c, d - 1, currShipChar, coordOfCurrentShip, boardCols, boardRows, boardDepth);
	}
}


std::set<Coordinate> BattleshipBoard::getNearbyCoordinates(Coordinate coord) const
/* we check in this function every coordinate seperatly */
{
	int r = coord.row;
	int c = coord.col;
	int d = coord.depth;
	std::set<Coordinate> adjCoordSet;

	if (isCoordianteInBoard(r - 1, c, d)) adjCoordSet.insert({ r - 1, c, d });    
	if (isCoordianteInBoard(r + 1, c, d)) adjCoordSet.insert({ r + 1, c, d });
	if (isCoordianteInBoard(r, c - 1, d)) adjCoordSet.insert({ r, c - 1, d });
	if (isCoordianteInBoard(r, c + 1, d)) adjCoordSet.insert({ r, c + 1, d });
	if (isCoordianteInBoard(r, c, d - 1)) adjCoordSet.insert({ r, c, d - 1 });
	if (isCoordianteInBoard(r, c, d + 1)) adjCoordSet.insert({ r, c, d + 1 });

	return std::set<Coordinate>(adjCoordSet);
}


bool BattleshipBoard::isPlayerShip(const int playerId, const char shipChar)
{
	if (playerId == PLAYERID_A && isupper(shipChar)) {
		return true;
	}
	else if (playerId == PLAYERID_B && islower(shipChar)) {
		return true;
	}
	else {
		return false;
	}
}

char BattleshipBoard::operator()(int r, int c, int d)const
{
	if (isCoordianteInBoard(r, c, d)) {
		return boardVec.at(BattleshipGameUtils::calcCoordIndex(r, c, d, rows, cols));
	}
	else return BLANK_CHAR;
}

void BattleshipBoard::setCoord(int r, int c, int d, char ch)
{
	if (!isCoordianteInBoard(r, c, d)) return;

	boardVec[BattleshipGameUtils::calcCoordIndex(r, c, d, rows, cols)] = ch;
}


/* this is strange to return reference to a vector, so we will return by value, and then we will make move to prevent new allocation */
std::vector<char> BattleshipBoard::InitNewEmptyBoardVector(int rows, int cols, int depths)
{
	std::vector<char> vec(rows * cols * depths, BLANK_CHAR);
	return vec;
}


void BattleshipBoard::CopyInputLineToBoard(const std::string & line, int currDepth, int currRow)
{
	size_t lineLen;
	if (line.length() < unsigned(cols))
		lineLen = line.length();
	else
		lineLen = cols;

	for (auto j = 0; j < lineLen; j++)
	{
		if (IsShipCharInBoard(line[j])) {
			setCoord(currRow, j, currDepth, line[j]);	/*else it will remain ' ' */
		}
	}
}

bool BattleshipBoard::parseBoardDimensions(std::string& line)
{
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);  //to support 'x' and 'X'
	
	std::vector<int> dimsVec;
	std::vector<std::string> tokens;
	char* stringEnd = nullptr;

	BattleshipGameUtils::splitStringByToken(line, BOARD_DIM_DELIMITER, std::back_inserter(tokens));

	for (auto tok : tokens)
	{
		int num = strtol(tok.c_str(), &stringEnd, 10);
		
		if (num < 1 || *stringEnd) return false;

		dimsVec.push_back(num);
	}

	if (dimsVec.size() < 3) return false;

	cols = dimsVec[0];
	rows = dimsVec[1];
	depth = dimsVec[2];

	return true;

}


void BattleshipBoard::countShipsTypes(const std::set<std::pair<char, std::set<Coordinate>>>& allShipsDetails, std::vector<std::pair<int, int>>& shipsCountVec) {

	int count_b = 0;//lenght = 1
	int count_p = 0;//lenght = 2
	int count_m = 0;//lenght = 3
	int count_d = 0;//lenght = 4

	for (auto shipType : allShipsDetails) {

		char ch = tolower(shipType.first);

		if (ch == tolower(RUBBER_BOAT)) count_b++;
		if (ch == tolower(SUBMARINE)) count_m++;
		if (ch == tolower(ROCKET_SHIP)) count_p++;
		if (ch == tolower(DESTROYER)) count_d++;
	}

	shipsCountVec.push_back(std::make_pair(1, count_b));
	shipsCountVec.push_back(std::make_pair(2, count_p));
	shipsCountVec.push_back(std::make_pair(3, count_m));
	shipsCountVec.push_back(std::make_pair(4, count_d));
}