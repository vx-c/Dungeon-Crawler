#pragma once
#include "GameState.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;



/* MainMenuState, the first state shown on launch. Should include things like game settings, saved data loading, and starting/quitting the game.
* not fully implemented. */
class MainMenuState :
    public GameState
{
	// IDs for events
	enum
	{
		GUI_ID_QUIT_BUTTON,
		GUI_ID_START_BUTTON
	};

	// the event receiver class for this gamestate
	class MyEventReceiver : public irr::IEventReceiver
	{
		irr::IrrlichtDevice *device;

		MainMenuState &parent;
		
	public:

		MyEventReceiver(MainMenuState &parent, irr::IrrlichtDevice &device);

		// handle events
		virtual bool OnEvent(const irr::SEvent &event);

	};


	MyEventReceiver *eventReceiver;


public:

	MainMenuState(irr::IrrlichtDevice &device);

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



};

