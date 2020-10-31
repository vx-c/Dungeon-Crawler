#pragma once

#include "GameState.h"
#include "MainMenuState.h"
#include "DungeonState.h"
#include "PauseMenuState.h"
#include "BattleState.h"

#include <irrlicht.h>

#include <SFML/System.hpp>

// Main game class. This manages the different gamestates and works as the main game loop through its update function.
class Game 
{
	// is the game running?
	bool running;

	// game states
	GameState *currentState;

	MainMenuState *mainMenuState;
	DungeonState *dungeonState;
	PauseMenuState *pauseMenuState;
	BattleState *battleState;

	// for calculating deltatime
	sf::Clock clock;
	float lastTime;

	// manager objects
	SoundManager soundManager;

	// irrlicht
	irr::IrrlichtDevice *device;

	int lastFPS;
	
	// render gamestates and the window
	void render();

	// clears the scenemanager and guienvironment so we can have a fresh scene for state changes
	void clearDevice();

public:
	Game();

	// starts the game
	void initialize();

	// boolean is the game running
	bool isRunning();

	// the main game loop function
	void update();

	// closes the game, deallocates memory
	void cleanup();

};

