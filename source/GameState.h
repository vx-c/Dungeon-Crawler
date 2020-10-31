#pragma once

#include "GameStateType.h"

#include <irrlicht.h>

// base class for actual GameState classes. Should not actually be instantiated.
class GameState
{

protected:
	GameStateType nextState;
	bool finished;

	irr::IrrlichtDevice *device;

public:

	// load assets, start the state
	virtual void initializeScene() = 0;

	// update, should be called once per frame, deltaTime in seconds
	virtual void update(float deltaTime) = 0;

	// cleanup memory, should be called when you're done with the state
	virtual void cleanup() = 0;

	// is the state no longer needed?
	virtual bool isFinished();

	// the state for the next frame
	virtual GameStateType getNextState();

	/* resets the nextState, this is needed because of when and how irrlicht handles events
	* call this function after the rendering is finished */
	void resetNextState();

	// returns the GameState type
	virtual GameStateType type() = 0;

	// prepare device to be rendered
	virtual void preRender() = 0;
};

