#include "DungeonState.h"

#include "DebugMacro.h"

#include "EnemyUnit.h"

#include <stdlib.h> // rand, srand

DungeonState::DungeonState(irr::IrrlichtDevice &device, SoundManager &soundManager, std::string dungeonMapPath)
	: dungeonMap(device, dungeonMapPath), soundManager(soundManager)
{
	using namespace irr;
	using namespace scene;
	using namespace core;
	using namespace video;

	nextState = GameStateType::DungeonState;
	finished = false;
	this->device = &device;

	eventReceiver = nullptr;

	camera = nullptr;

	keybinds = Keybinds();

	// initial player state
	playerState = PlayerState::standing;
	playerFacing = dungeonMap.getStartFacing();
	radiansRotated = 0;
	moveProgress = 0;
	wallBumped = false;

	playerPosition = vector2di(0,0);
}

DungeonState::~DungeonState()
{
	delete eventReceiver;
	eventReceiver = nullptr;
}

void DungeonState::initializeScene(bool totalReset)
{
	using namespace irr;
	using namespace scene;
	using namespace core;
	using namespace video;

	// initialize the dungeon map
	dungeonMap.initializeScene();

	// set the mouse cursor invisible in this scene
	device->getCursorControl()->setVisible(false);

	// play the background music
        soundManager.playMusic(MusicType::Bgm);

	// set the event receiver
	eventReceiver = new MyEventReceiver(*this, *device);
	device->setEventReceiver(eventReceiver);

	// setup the camera
	camera = this->device->getSceneManager()->getActiveCamera();
	camera = device->getSceneManager()->addCameraSceneNode();

	vector3df camPosition;
	vector3df initialTarget;
	Directions::Value initialDirection;
	
	if (totalReset)
	{
		camPosition = dungeonMap.getCamStartPosition();
		playerPosition = dungeonMap.getStartPosition();

		initialDirection = dungeonMap.getStartFacing();
	}
	else
	{
		camPosition = dungeonMap.getPosition(playerPosition.X, playerPosition.Y);

		initialDirection = playerFacing;
	}
	camera->setPosition(camPosition);
	camera->setTarget(camera->getPosition() + directions.getVectorFloat(initialDirection));

	// TODO add lighting
}


void DungeonState::update(float deltaTime)
{
	std::cout << playerPosition.X << ", " << playerPosition.Y << std::endl;

	nextState = type();

	handleInput(deltaTime);

	// update the player/camera
	switch (playerState)
	{
	case PlayerState::standing:
		break;

	case PlayerState::turningLeft:
		turnLeft(dungeonMap.getTurnSpeed(), deltaTime);
		break;

	case PlayerState::turningRight:
		turnRight(dungeonMap.getTurnSpeed(), deltaTime);
		break;

	case PlayerState::movingForward:
		move(dungeonMap.getMoveSpeed(), deltaTime, playerFacing);
		break;

	case PlayerState::movingBackward:
		move(dungeonMap.getMoveSpeed(), deltaTime, directions.getReverse(playerFacing));
		break;

	case PlayerState::wallBumpForward:
		wallBump(dungeonMap.getBumpDistance(), dungeonMap.getBumpSpeed(), 
                        deltaTime, playerFacing);
		break;

	case PlayerState::wallBumpBackward:
		wallBump(dungeonMap.getBumpDistance(), dungeonMap.getBumpSpeed(), 
                        deltaTime, directions.getReverse(playerFacing));
		break;
	}
}

void DungeonState::cleanup()
{

}


GameStateType DungeonState::type()
{
	return GameStateType::DungeonState;
}


void DungeonState::preRender()
{

}

void DungeonState::randomBattle()
{
    // TODO don't hardcode the chance for a battle
    int num = rand () % 100 + 1;
    if (num <= 5)
    {
        nextState = GameStateType::BattleState;
        soundManager.stopMusic(MusicType::Bgm);
        std::cout << "battle started/n";
    }
}

// camera functions //////////////////////////////////////////////////////////////////////////////////
void DungeonState::translateCamera(irr::core::vector3df displacement)
{
	camera->setPosition(camera->getPosition() + displacement);
	camera->setTarget(camera->getTarget() + displacement);
}

void DungeonState::rotateCamera(float angle)
{
	float currentAngle = getCameraAngle();

	angle += currentAngle;

	camera->setTarget(camera->getPosition() + irr::core::vector3df({cosf(angle), 0,
                    sinf(angle)}));
}

float DungeonState::getCameraAngle()
{
	return atan2f(camera->getTarget().Z - camera->getPosition().Z, 
                camera->getTarget().X - camera->getPosition().X);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DungeonState::handleInput(float deltaTime)
{
	using namespace irr;

	// 	add some sort of action buffer or stack 
        // 	so that keys hit during player actions aren't totally ignored

	if (eventReceiver->IsKeyDown(keybinds.turnLeft))
	{
		if (playerState == PlayerState::standing)
		{
			soundManager.makeSound(SoundType::Turn);
			playerState = PlayerState::turningLeft;
		}
	}
	if (eventReceiver->IsKeyDown(keybinds.turnRight))
	{
		if (playerState == PlayerState::standing)
		{
			soundManager.makeSound(SoundType::Turn);
			playerState = PlayerState::turningRight;
		}
	}
	if (eventReceiver->IsKeyDown(keybinds.moveForward) && playerState == PlayerState::standing)
	{
		if (isMoveValid(playerFacing))
		{
			soundManager.makeSound(SoundType::Move);
			playerState = PlayerState::movingForward;
		}
		else
		{
			playerState = PlayerState::wallBumpForward;
			soundManager.makeSound(SoundType::WallBump);
		}
	}
	if (eventReceiver->IsKeyDown(keybinds.moveBackward) && playerState == PlayerState::standing)
	{
		if (isMoveValid(directions.getReverse(playerFacing)))
		{
			soundManager.makeSound(SoundType::Move);
			playerState = PlayerState::movingBackward;
		}
		else
		{
			playerState = PlayerState::wallBumpBackward;
			soundManager.makeSound(SoundType::WallBump);
		}
	}
	if (eventReceiver->IsKeyDown(keybinds.exitGame))
	{
		device->closeDevice();
	}

	// TEMP for testing
	if (eventReceiver->IsKeyDown(keybinds.startBattle) && playerState == PlayerState::standing)
	{
		nextState = GameStateType::BattleState;
                soundManager.stopMusic(MusicType::Bgm);
		std::cout << "battle key clicked\n";
	}

}

// player action functions /////////////////////////////////////////////////////////////

void DungeonState::move(float speed, float deltaTime, Directions::Value direction)
{
	irr::core::vector3df vec = directions.getVectorFloat(direction);
	vec.normalize();
	vec = vec * dungeonMap.getScaling() * (deltaTime / speed);

	// NOTE the getScaling.X here assumes that XZ scaling are the 
        // same which they always will be as of right now
	// I can't imagine any reason to change that because 
        // it would make the game feel terrible but I should mention it either way
	if (vec.getLength() + moveProgress >= dungeonMap.getScaling().X)
	{
		// were done moving
		vec = directions.getVectorFloat(direction).normalize();
		vec = vec * (dungeonMap.getScaling() - moveProgress);

		translateCamera(vec);
		playerState = PlayerState::standing;
		moveProgress = 0;
                
                // chance for a random battle
                randomBattle();
                
		// -directions.getVectorInt(direction).Z // make sure you put the negative here 
                // if you're doing something similar elsewhere
		playerPosition = irr::core::vector2di(
                        directions.getVectorInt(direction).X + playerPosition.X, 
                        -directions.getVectorInt(direction).Z + playerPosition.Y
                );
	}
	else
	{
		translateCamera(vec);
		moveProgress += vec.getLength();
	}

}

bool DungeonState::isMoveValid(Directions::Value direction)
{
	irr::core::vector3df vec = directions.getVectorFloat(direction);

	vec = vec * dungeonMap.getScaling();

	translateCamera(vec);

	// if the player is trying to move into a wall
	if (!dungeonMap.isWallsPositionEmpty( {camera->getPosition().X, camera->getPosition().Z} ))
	{
		DEBUG_MODE(printf("there's a wall in the way\n"));
		// make sure to fix the camera first
		translateCamera(-vec); 
		return false;
	}
	else
	{
		// make sure to fix the camera first
		translateCamera(-vec);
		return true;
	}
}

void DungeonState::turnLeft(float speed, float deltaTime)
{
	float angle = irr::core::HALF_PI * (deltaTime / speed);

	if (radiansRotated + angle >= irr::core::HALF_PI)
	{
		// we're done turning

		// update the player facing and the camera target

		playerFacing = directions.getLeft(playerFacing);

		irr::core::vector3df target = directions.getVectorFloat(playerFacing);

		camera->setTarget(camera->getPosition() + target);

		playerState = PlayerState::standing;

		radiansRotated = 0; 
	}
	else
	{
		rotateCamera(angle);
		radiansRotated += angle;
	}
}

void DungeonState::turnRight(float speed, float deltaTime)
{
	float angle = irr::core::HALF_PI * (deltaTime / speed);

	if (radiansRotated + angle >= irr::core::HALF_PI)
	{
		// we're done turning

		// update the player facing and the camera target

		playerFacing = directions.getRight(playerFacing);

		irr::core::vector3df target = directions.getVectorFloat(playerFacing);
		
		camera->setTarget(camera->getPosition() + target);

		playerState = PlayerState::standing;

		radiansRotated = 0; 
	}
	else
	{
		rotateCamera(-angle);
		radiansRotated += angle;
	}

}

void DungeonState::wallBump(float distance, float speed, float deltaTime, Directions::Value direction)
{
	irr::core::vector3df vec = directions.getVectorFloat(direction);
	vec.normalize();
	vec = vec * distance * (deltaTime / speed);

	if (!wallBumped)
	{
		if (vec.getLength() + moveProgress >= distance)
		{
			// were done moving forward
			vec = directions.getVectorFloat(direction).normalize();
			vec = vec * (distance - moveProgress);

			translateCamera(vec);
			moveProgress = 0;

			wallBumped = true;
		}
		else
		{
			translateCamera(vec);
			moveProgress += vec.getLength();
		}
	}
	if (wallBumped)
	{
		if (vec.getLength() + moveProgress >= distance)
		{
			// were done moving backward
			vec = directions.getVectorFloat(direction).normalize();
			vec = vec * (distance - moveProgress);

			translateCamera(-vec);
			playerState = PlayerState::standing;
			moveProgress = 0;
			wallBumped = false;;
		}
		else
		{
			translateCamera(-vec);
			moveProgress += vec.getLength();
		}
	}
}

void DungeonState::readyStateChange()
{
	resetNextState();
	eventReceiver->ResetKeys();
}

std::vector<EnemyUnit> DungeonState::getEnemies()
{
        //TODO generate enemies based on dungeon with rng
        std::vector<EnemyUnit> enemies;

        enemies.push_back(EnemyUnit(L"Skeleton 1", Stats(10,10,3,3,1, 35)));
        enemies.push_back(EnemyUnit(L"Skeleton 2", Stats(10,10,3,3,2, 35)));
        enemies.push_back(EnemyUnit(L"Skeleton 3", Stats(10,10,3,3,3, 35)));

        return enemies;
}

// MyEventReceiver functions /////////////////////////////////////////////////////////////////////////

DungeonState::MyEventReceiver::MyEventReceiver(DungeonState &parent, irr::IrrlichtDevice &device)
	: parent(parent), device(&device)
{
	for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
	{
		keyIsDown[i] = false;
	}
}

bool DungeonState::MyEventReceiver::OnEvent(const irr::SEvent &event)
{
	using namespace irr;
	using namespace io;

	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		keyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
	}

	return false;
}

bool DungeonState::MyEventReceiver::IsKeyDown(irr::EKEY_CODE keyCode) const
{
	return keyIsDown[keyCode];
}

void DungeonState::MyEventReceiver::ResetKeys()
{
	for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
	{
		keyIsDown[i] = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// Keybinds functions ////////////////////////////////////////////////////////////////////////////////

DungeonState::Keybinds::Keybinds()
{
	using namespace irr;

	// TODO load keybinds from a settings file, don't hardcode them
	moveForward = KEY_UP;
	moveBackward = KEY_DOWN;
	turnLeft = KEY_LEFT;
	turnRight = KEY_RIGHT;
	exitGame = KEY_ESCAPE;

	// TEMP for testing
	startBattle = irr::KEY_KEY_B;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
