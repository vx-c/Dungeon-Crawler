#include "GameState.h"

#include <iostream>

GameStateType GameState::getNextState()
{
	return nextState;
}

bool GameState::isFinished()
{
	return finished;
}

void GameState::resetNextState()
{
	nextState = type();
}

void GameState::readyStateChange()
{
	resetNextState();
}