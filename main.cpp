#include "Game.h"

#include "DebugMacro.h"

#include <SFML/Audio.hpp>

int main()
{
//	FreeConsole(); // for windows release 
	
	Game game = Game();

	game.initialize();

	while (game.isRunning())
	{
		game.update();

	}
	
	game.cleanup();

	return 0;
}



