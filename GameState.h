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

	// resets the nextState
	void resetNextState();

public:

	// load assets, start the state
	virtual void initializeScene(bool totalReset) = 0;

	// update, should be called once per frame, deltaTime in seconds
	virtual void update(float deltaTime) = 0;

	// cleanup memory, should be called when you're done with the state
	virtual void cleanup() = 0;

	// is the state no longer needed?
	virtual bool isFinished();

	// the state for the next frame
	virtual GameStateType getNextState();

	// returns the GameState type
	virtual GameStateType type() = 0;

	// prepare device to be rendered
	virtual void preRender() = 0;

	// clean up whatever needs to be done before the state changes to something else
	virtual void readyStateChange();
};

