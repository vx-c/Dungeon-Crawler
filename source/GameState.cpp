#include "GameState.h"

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