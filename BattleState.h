#pragma once
#include "GameState.h"

// the state used for battles in the game, not fully implemented
class BattleState :
    public GameState
{


public:

	BattleState(irr::IrrlichtDevice &device);

	// load assets, start the state
	void initializeScene();

	// update, should be called once per frame, deltatime is in seconds
	void update(float deltaTime);

	// cleanup memory, should be called when you're done with the state
	void cleanup();

	// returns the GameState type
	GameStateType type();

	// prepare device to be rendered
	void preRender();



};

