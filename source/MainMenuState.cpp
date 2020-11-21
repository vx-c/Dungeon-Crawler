#include "MainMenuState.h"

#include "DebugMacro.h"
#include <iostream>

#include <irrlicht.h>

#include "GameStateType.h"

MainMenuState::MainMenuState(irr::IrrlichtDevice &device)
{
	finished = false;
	this->device = &device;

	nextState = GameStateType::MainMenuState;

	eventReceiver = nullptr;
}


void MainMenuState::initializeScene(bool totalReset)
{
	// add buttons
	// TODO center the buttons and make the main menu pretty
	// TODO don't hardcode these parameters
	device->getGUIEnvironment()->addButton(rect<s32>(520, 200, 770, 300), 0, GUI_ID_START_BUTTON,
		L"Start Game", L"Start the game");
	device->getGUIEnvironment()->addButton(rect<s32>(520, 400, 770, 500), 0, GUI_ID_QUIT_BUTTON,
		L"Quit", L"Exit the game");

	// set the event receiver
	eventReceiver = new MyEventReceiver(*this, *device);
	device->setEventReceiver(eventReceiver);

	// set the mouse cursor visible in this scene
	device->getCursorControl()->setVisible(true);
}


void MainMenuState::update(float deltaTime)
{
//	nextState = type();

//	DEBUG_MODE(printf("mainmenu state\n"));

}


void MainMenuState::cleanup()
{
	delete eventReceiver;
	eventReceiver = nullptr;

	device = nullptr;
}

GameStateType MainMenuState::type()
{
	return GameStateType::MainMenuState;
}

void MainMenuState::preRender()
{

}


// MyEventReceiver functions ////////////////////////////////////////////////////////////
MainMenuState::MyEventReceiver::MyEventReceiver(MainMenuState &parent, irr::IrrlichtDevice &device)
	: parent(parent), device(&device)
{

}

bool MainMenuState::MyEventReceiver::OnEvent(const irr::SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		irr::s32 id = event.GUIEvent.Caller->getID();
		
		switch (event.GUIEvent.EventType)
		{
		case EGET_BUTTON_CLICKED:
			switch (id)
			{
			case GUI_ID_START_BUTTON:
				DEBUG_MODE(std::cout << "start clicked\n");
				// TEMPORARY WERE USING THIS TO TEST COMBAT
				parent.nextState = GameStateType::DungeonState;
				return true;
			
			case GUI_ID_QUIT_BUTTON:
				
				DEBUG_MODE(std::cout << "quit clicked\n");
				device->closeDevice();
				return true;
			
			}
				
		}
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////