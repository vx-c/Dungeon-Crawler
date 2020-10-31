#include "BattleState.h"

#include "DebugMacro.h"

BattleState::BattleState(irr::IrrlichtDevice &device) 
{
	nextState = GameStateType::BattleState;
	finished = false;
	this->device = &device;
}


void BattleState::initializeScene()
{

}


void BattleState::update(float deltaTime)
{
	nextState = type();

	// temp
	DEBUG_MODE(printf("BattleState\n"));
	nextState = GameStateType::DungeonState;
	finished = true;
}


void BattleState::cleanup()
{

}

GameStateType BattleState::type()
{
	return GameStateType::BattleState;
}

void BattleState::preRender()
{

}
