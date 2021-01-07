#include "Game.h"
#include "GameState.h"

#include "DebugMacro.h"

#include <irrlicht.h>

#include <stdlib.h>
#include <time.h>

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

	// TODO load this from a save file or something
	// TEMP for testing we'll hardcode the party here for now
	party.push_back(PlayerUnit(L"John", Stats(5,3,3,3,1,60)));
	party.push_back(PlayerUnit(L"Rumia", Stats(5,3,3,3,2,35)));
	party.push_back(PlayerUnit(L"Schoolgirl", Stats(5,3,3,3,3,22)));
	party.push_back(PlayerUnit(L"Badass Knight", Stats(5,3,3,3,4,1000)));
	party.push_back(PlayerUnit(L"416", Stats(5,3,3,3,5,40)));

	party[0].addSkill(Skills::fireball);
	party[0].addSkill(Skills::firewave);
	party[1].addSkill(Skills::firewave);
	party[1].addSkill(Skills::basicAttack);
	party[2].addSkill(Skills::basicAttack);
	party[2].addSkill(Skills::firewave);
	party[3].addSkill(Skills::basicAttack);
	party[3].addSkill(Skills::fireball);
	party[4].addSkill(Skills::fireball);
	party[4].addSkill(Skills::basicAttack);

        srand(time(NULL));
}

void Game::initialize() 
{
	using namespace irr;

	running = true;
	
	// TODO settings shouldn't be hardcoded
	device = irr::createDevice(video::EDT_OPENGL, core::dimension2d<u32>(1280, 720), 16, false, false, false, 0); 
	device->setWindowCaption(L"[Dungeon Crawler]");

//	device->getVideoDriver()->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true); // TEMP

	// start the game in the main menu state
	mainMenuState = new MainMenuState(*device);
	currentState = mainMenuState;
	currentState->initializeScene(true);

	lastFPS = 0;

	// start the sfml clock
	clock.restart();
	lastTime = clock.getElapsedTime().asSeconds();

	// TODO don't hardcode sounds.json path
	soundManager.initialize("media/sounds.json");
}

bool Game::isRunning() 
{
	return running;
}

void Game::update()
{
	// TODO settings shouldn't be hardcoded
	// we start the scene here
	device->getVideoDriver()->beginScene(true,true, video::SColor(255,20,20,20)); 

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
		// reset the next state, we don't want to change the state without 
                // resetting this or we will be stuck changing states
		currentState->readyStateChange();

		DEBUG_MODE(printf("state change\n"));

		// if the current state is finished free up the memory
		
                // TODO i may want to make a function or 
                // something for this so i'm not repeating the code
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
		// TODO i should make a function or something for this so i'm not repeating the code
		
                std::vector<EnemyUnit> enemies; //for battlestate initalization

                switch (nextState)
		{
		case GameStateType::MainMenuState:
                        
			clearDevice();

                        // we restart the game when we change to this state
                        // TODO we may want a function for this later
                        delete dungeonState;
                        dungeonState = nullptr;
                        delete battleState;
                        battleState = nullptr;
                        delete pauseMenuState;
                        pauseMenuState = nullptr;
                        delete mainMenuState;
                        mainMenuState = nullptr;

			if (mainMenuState == nullptr)
			{
				mainMenuState = new MainMenuState(*device);
				mainMenuState->initializeScene(true);
			}
			else
			{
				mainMenuState->initializeScene(false);
			}
			currentState = mainMenuState;
			break;

		case GameStateType::DungeonState:
			clearDevice();
			if (dungeonState == nullptr)
			{
				// TODO the dungeonMapPath will need to be variable, 
                                // don't hardcode things like this.
				dungeonState = new DungeonState(*device, 
                                        soundManager, "media/dungeon.json");
				dungeonState->initializeScene(true);
			}
			else
			{
				dungeonState->initializeScene(false);
			}
			currentState = dungeonState;
			break;

		case GameStateType::BattleState:
			
                        enemies = dungeonState->getEnemies();

                        clearDevice();
			if (battleState == nullptr)
			{
				battleState = new BattleState(*device, soundManager,
                                        &party, enemies);
				battleState->initializeScene(true);
			}
			else
			{
				battleState->initializeScene(false);
			}
			currentState = battleState;
			break;

		case GameStateType::PauseMenuState:
			clearDevice();
			if (pauseMenuState == nullptr)
			{
				pauseMenuState = new PauseMenuState(*device);
				pauseMenuState->initializeScene(true);
			}
			else
			{
				pauseMenuState->initializeScene(false);
			}
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
		// if the device isn't running (device->closeDevice() was called) 
                // then we cleanup and close the game
		cleanup();
		running = false;
		return;
	}

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
