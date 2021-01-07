#include "MainMenuState.h"

#include "DebugMacro.h"
#include <iostream>

#include <irrlicht.h>

#include "GameStateType.h"

#include "MyJson.h"

#include <fstream>

#include "StringConversion.h"


MainMenuState::MainMenuState(irr::IrrlichtDevice &device)
{
	finished = false;
	this->device = &device;

	nextState = GameStateType::MainMenuState;

	eventReceiver = nullptr;
}


void MainMenuState::initializeScene(bool totalReset)
{ 
        // load the json file
        std::ifstream file("media/mainmenu.json", std::ifstream::binary);
        Json::Value root;
        file >> root;
        file.close();

        // for convenience
        using namespace irr;
        using namespace core;
        using namespace io;
        auto *gui = device->getGUIEnvironment();
        auto *driver = device->getVideoDriver();

        // set the gui font
        auto *font = gui->getFont(MyJson::get(root,"fontPath").asString().c_str());
        if (font)
        {
                gui->getSkin()->setFont(font);
        }
        else
        {
                DEBUG_MODE(printf("error loading font\n"));
        }

        // set buttons text color
        auto colors = MyJson::get(root, "buttonTextColor");
        gui->getSkin()->setColor(gui::EGUI_DEFAULT_COLOR::EGDC_BUTTON_TEXT,
                irr::video::SColor(colors[0].asInt(), colors[1].asInt(),
                    colors[2].asInt(), colors[3].asInt()));
        
        // button images
        auto buttonImage = driver->getTexture(stringToWString(MyJson::get(root,
                        "buttonImage").asString()).c_str());
        auto buttonPressedImage = driver->getTexture(stringToWString(MyJson::get(root,
                        "buttonPressedImage").asString()).c_str());

	// add buttons
	// TODO don't hardcode these parameters
	auto button = device->getGUIEnvironment()->addButton(rect<s32>(507, 200, 793, 280), 0,
                GUI_ID_START_BUTTON, L"Start Game", L"Start the game");
        button->setImage(buttonImage);
        button->setPressedImage(buttonPressedImage);

	button = device->getGUIEnvironment()->addButton(rect<s32>(507, 400, 793, 480), 0, 
                GUI_ID_QUIT_BUTTON, L"Quit", L"Exit the game");
        button->setImage(buttonImage);
        button->setPressedImage(buttonPressedImage);

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
