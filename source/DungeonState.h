#pragma once
#include "GameState.h"

#include "DungeonMap.h"

// the state used for the dungeon crawling part of the game, not fully implemented
class DungeonState :
    public GameState
{
	// loads and stores keybind information
	struct Keybinds
	{
		irr::EKEY_CODE moveForward, moveBackward, turnLeft, turnRight, exitGame;
		
		// TEMP for testing
		irr::EKEY_CODE startBattle;

		Keybinds();
	};

	enum class PlayerState
	{
		movingForward, movingBackward,
		wallBumpForward, wallBumpBackward,
		turningLeft, turningRight, 
		standing
	};

	// the event receiver class for this gamestate
	class MyEventReceiver : public irr::IEventReceiver
	{
		irr::IrrlichtDevice *device;

		DungeonState &parent;

		// we use this array to keep track of which keys are down and which keys are up
		bool keyIsDown[irr::KEY_KEY_CODES_COUNT];

	public:
		MyEventReceiver(DungeonState &parent, irr::IrrlichtDevice &device);

		// called when an event occurs
		virtual bool OnEvent(const irr::SEvent &event);

		// used to tell if a key is being held down
		virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const;

		// resets all key presses
		void ResetKeys();

	};

	Keybinds keybinds;

	MyEventReceiver *eventReceiver;

	DungeonMap dungeonMap;

	SoundManager &soundManager;

	// player and camera related members
	PlayerState playerState;
	irr::scene::ICameraSceneNode *camera;
	Directions::Value playerFacing;
	irr::core::vector3df playerPosition; // TODO this should be 2d

	Directions directions;

	// used to keep track of player action progress
	float radiansRotated; 
	float moveProgress;
	bool wallBumped; // used to keep track of where we are in the wallBump

	// player action functions//////////////////////////////////////////////

	// the player moves in the direction
	void move(float speed, float deltaTime, Directions::Value direction);

	// the player turns left, speed is the number of seconds to complete the turn, deltatime is in seconds
	void turnLeft(float speed, float deltaTime);

	// the player tries to turn right, speed is the number of seconds to complete the turn, deltatime is in seconds
	void turnRight(float speed, float deltaTime);

	// checks if a move is valid
	bool isMoveValid(Directions::Value direction);

	// the player bangs into a wall
	void wallBump(float distance, float speed, float deltaTime, Directions::Value direction);

	/////////////////////////////////////////////////////////////////////////

	// camera functions /////////////////////////////////////////////////////
	// move the camera
	void translateCamera(irr::core::vector3df displacement);

	// rotate the camera counterclockwise about the Y axis, angle is in radians,
	void rotateCamera(float angle);

	// returns the camera's current angle in radians
	float getCameraAngle();
	/////////////////////////////////////////////////////////////////////////


public:

	DungeonState(irr::IrrlichtDevice &device, SoundManager &soundManager, const std::string &dungeonMapPath);
	~DungeonState();

	// load assets, start the state
	void initializeScene(bool totalReset);

	// update, should be called once per frame, deltatime is in seconds
	void update(float deltaTime);

	// cleanup memory, should be called when you're done with the state
	void cleanup();

	// returns the GameState type
	GameStateType type();

	// prepare device to be rendered
	void preRender();

	// resets and cleans up some data after rendering in preparation for the next frame
	void postRender();

	// handle player input
	void handleInput(float deltaTime);

	// clean up whatever needs to be done before the state changes to something else
	void readyStateChange();
};

