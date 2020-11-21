#pragma once
#include "GameState.h"

/* the state used for the pause menu, includes inventory, party management, skills, as well as things like settings, and exiting the game.
*  not fully implemented */
class PauseMenuState :
    public GameState
{


public:

	PauseMenuState(irr::IrrlichtDevice &device);

	// load assets, start the state
	void initializeScene(bool totalReset);

	// update, should be called once per frame, deltatime is in seconds
	void update(float deltaTime);

	// cleanup memory, should be called when you're done with the state
	void cleanup();

	// returns the GameState type
	GameStateType type();

	// prepare device to be rendered
	void preRender();



};

