#include "PauseMenuState.h"

#include "DebugMacro.h"

PauseMenuState::PauseMenuState(irr::IrrlichtDevice &device)
{
	nextState = GameStateType::PauseMenuState;
	finished = false;
	this->device = &device;
}

void PauseMenuState::initializeScene(bool totalReset)
{

}


void PauseMenuState::update(float deltaTime)
{
	nextState = type();

	// temp
	DEBUG_MODE(printf("PauseMenuState\n"));
	nextState = GameStateType::DungeonState;
	finished = true;
}


void PauseMenuState::cleanup()
{

}

GameStateType PauseMenuState::type()
{
	return GameStateType::PauseMenuState;
}

void PauseMenuState::preRender()
{

}

