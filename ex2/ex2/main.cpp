#include "BattleshipGameManager.h"

int main(int argc, char* argv[])
{
	try
	{
		BattleshipGameManager Game(argc, argv);
		if (Game.isGameSuccessfullyCreated()){
			Game.Run();	
		}
		else {
			return -1;
		}
	}
	catch (std::exception ex)
	{
		return -1;
	}

	return 0;
 }
