#include "Game.h"

#include "DebugMacro.h"

#include <SFML/Audio.hpp>

#include <Windows.h>

int main()
{
	FreeConsole();
	
	Game game = Game();

	game.initialize();

	while (game.isRunning())
	{
		game.update();

	}
	
	game.cleanup();

	return 0;
}



