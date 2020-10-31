#include "Game.h"
#include "GameState.h"

#include "DebugMacro.h"

#include <irrlicht.h>

Game::Game()
{
	running = false;

	currentState = nullptr;

	mainMenuState = nullptr;
	dungeonState = nullptr;
	pauseMenuState = nullptr;
	battleState = nullptr;

	device = nullptr;

	lastFPS = 0;
	lastTime = 0;

}

void Game::initialize() 
{
	using namespace irr;

	running = true;
	
	// TODO settings shouldn't be hardcoded
	device = createDevice(video::EDT_DIRECT3D9, core::dimension2d<u32>(1280, 720), 16, false, false, false, 0); 
	device->setWindowCaption(L"[Dungeon Crawler]");

	// start the game in the main menu state
	mainMenuState = new MainMenuState(*device);
	currentState = mainMenuState;
	currentState->initializeScene();

	lastFPS = 0;

	// start the sfml clock
	clock.restart();
	lastTime = clock.getElapsedTime().asSeconds();

	// TODO don't hardcode sounds.txt path
	soundManager.initialize("mymedia/sounds.txt");
}

bool Game::isRunning() 
{
	return running;
}

void Game::update()
{
//	DEBUG_MODE(printf("update\n"));

	// calculate deltatime
	float currentTime = clock.getElapsedTime().asSeconds();
	float deltaTime =  currentTime - lastTime;
	lastTime = currentTime;

	GameStateType nextState;
	bool finishedState;
	
	currentState->update(deltaTime);
	nextState = currentState->getNextState();
	finishedState = currentState->isFinished();

	// change the current state if necessary
	if (nextState != currentState->type())
	{
		// reset the next state, we don't want to change the state without resetting this or we will be stuck changing states
		currentState->resetNextState(); 

		DEBUG_MODE(printf("state change\n"));

		// if the current state is finished free up the memory
		if (finishedState)
		{
			currentState->cleanup();

			switch (currentState->type())
			{
			case GameStateType::MainMenuState:
				mainMenuState->cleanup();
				delete mainMenuState;
				mainMenuState = nullptr;
				break;

			case GameStateType::DungeonState:
				dungeonState->cleanup();
				delete dungeonState;
				dungeonState = nullptr;
				break;

			case GameStateType::BattleState:
				battleState->cleanup();
				delete battleState;
				battleState = nullptr;
				break;

			case GameStateType::PauseMenuState:
				pauseMenuState->cleanup();
				delete pauseMenuState;
				pauseMenuState = nullptr;
				break;
			}

			currentState = nullptr;

		}

		// change the current state
		switch (nextState)
		{
		case GameStateType::MainMenuState:
			if (mainMenuState == nullptr)
			{
				mainMenuState = new MainMenuState(*device);
			}
			clearDevice();
			mainMenuState->initializeScene();
			currentState = mainMenuState;
			break;

		case GameStateType::DungeonState:
			if (dungeonState == nullptr)
			{
				// TODO the dungeonMapPath will need to be variable, don't hardcode things like this.
				dungeonState = new DungeonState(*device, soundManager, "mymedia/dungeon.txt");
			}
			clearDevice();
			dungeonState->initializeScene();
			currentState = dungeonState;
			break;

		case GameStateType::BattleState:
			if (battleState == nullptr)
			{
				battleState = new BattleState(*device);
			}
			clearDevice();
			battleState->initializeScene();
			currentState = battleState;
			break;

		case GameStateType::PauseMenuState:
			if (pauseMenuState == nullptr)
			{
				pauseMenuState = new PauseMenuState(*device);
				clearDevice();
				pauseMenuState->initializeScene();
			}
			clearDevice();
			pauseMenuState->initializeScene();
			currentState = pauseMenuState;
			break;
		}
	}

	// display the fps in the window caption
	int fps = device->getVideoDriver()->getFPS();

	if (fps != lastFPS)
	{
		// TODO don't hardcore the window caption
		irr::core::stringw windowCaption = "[Dungeon Crawler] FPS: ";
		windowCaption += fps;
		device->setWindowCaption(windowCaption.c_str());
		lastFPS = fps;
	}

	// TODO call this less frequently, it doesn't need to be called every frame
	soundManager.clear();

	render();
}

void Game::render()
{
//	DEBUG_MODE(printf("render"));

	using namespace irr;

    currentState->preRender();

	// device run has to be called to update the device
	if (!device->run())
	{
		// if the device isn't running (device->closeDevice() was called) then we cleanup and close the game
		cleanup();
		running = false;
		return;
	}

	// TODO settings shouldn't be hardcoded
	device->getVideoDriver()->beginScene(true,true, video::SColor(255,100,101,140)); 

	device->getSceneManager()->drawAll();
	device->getGUIEnvironment()->drawAll();

	device->getVideoDriver()->endScene();

}

void Game::clearDevice()
{
	device->getSceneManager()->clear();
	device->getSceneManager()->getMeshCache()->clear();
	device->getGUIEnvironment()->clear();
}

void Game::cleanup()
{
	currentState = nullptr;

	if (mainMenuState != nullptr)
	{
		mainMenuState->cleanup();
	}
	delete mainMenuState;
	mainMenuState = nullptr;
	
	if (dungeonState != nullptr)
	{
		dungeonState->cleanup();
	}
	delete dungeonState;
	dungeonState = nullptr;
	
	if (battleState != nullptr)
	{
		battleState->cleanup();
	}
	delete battleState;
	battleState = nullptr;

	if (pauseMenuState != nullptr)
	{
		pauseMenuState->cleanup();
	}
	delete pauseMenuState;
	pauseMenuState = nullptr;

	if (device != nullptr)
	{
		device->drop();
		device = nullptr;
	}
}