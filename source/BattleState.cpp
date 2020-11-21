#include "BattleState.h"

#include "DebugMacro.h"

#include "Unit.h"

#include <iostream>

#include <string>

#include "PlayerUnit.h"
#include "EnemyUnit.h"

#include <json/json.h>
#include <fstream>

#include <wchar.h>

#include "StringConversion.h"

#include "MyJson.h"

BattleState::BattleState(irr::IrrlichtDevice &device, std::vector<PlayerUnit> *party) 
	:	selectedSkill(&Skills::doNothing)
{
	nextState = GameStateType::BattleState;
	finished = false;
	this->device = &device;

	state = State::Main;

	actingUnit = nullptr;
	actionText = nullptr;
	eventReceiver = nullptr;
	skillsList = nullptr;

	// set playerUnits up with pointers to the party members
	for (size_t i = 0; i < party->size(); ++i)
	{
		playerUnits.push_back(&(*party)[i]);
	}
}

BattleState::~BattleState()
{
	delete eventReceiver;
	eventReceiver = nullptr;
}


void BattleState::initializeScene(bool totalReset)
{
	// for convenience
	using namespace irr;
	using namespace core;
	using namespace io;
	auto *gui = device->getGUIEnvironment();
	auto *driver = device->getVideoDriver();

	// load the json file
	// TODO don't hardcode the json path
	std::ifstream file("media/battle.json", std::ifstream::binary);
	Json::Value root;
	file >> root;
	file.close();

	// set the event receiver
	eventReceiver = new MyEventReceiver(*this, *device);
	device->setEventReceiver(eventReceiver);

	// set the mouse cursor visible in this scene
	device->getCursorControl()->setVisible(true);

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

	// add the action text box
	// TODO don't hardcode this
	actionText = gui->addStaticText(std::wstring(L"").c_str(),
		irr::core::rect<irr::s32>(500,430,828,480), true, true, 0, -1, true);

	initializeUnitInfo();

	initializeButtons();

	// set up the next actor queue and the currently acting unit
	refillNextActor();
	actingUnit = nextActor.top();
	nextActor.pop(); // don't forget this pop, otherwise there is an extra action
}

void BattleState::initializeButtons()
{
	// for convenience
	using namespace irr;
	using namespace core;
	using namespace io;
	auto *gui = device->getGUIEnvironment();
	auto *driver = device->getVideoDriver();

	// load the json file
	// TODO don't hardcode the json path
	std::ifstream file("media/battle.json", std::ifstream::binary);
	Json::Value root;
	file >> root;
	file.close();

	// set buttons text color
	auto colors = MyJson::get(root, "buttonTextColor");
	gui->getSkin()->setColor(gui::EGUI_DEFAULT_COLOR::EGDC_BUTTON_TEXT, 
		irr::video::SColor(colors[0].asInt(), colors[1].asInt(), colors[2].asInt(), colors[3].asInt()));

	// add buttons
	// TODO center the buttons and make things look good

	// button images
	auto buttonImage = driver->getTexture(stringToWString(MyJson::get(root,"attackButtonImage").asString()).c_str());
	auto buttonPressedImage = driver->getTexture(stringToWString(MyJson::get(root,"attackButtonPressedImage").asString()).c_str());

	// setup the attack button
	auto points = MyJson::get(root,"attackButtonRect");
	rect<s32> buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), points[2].asInt(), points[3].asInt());
	std::wstring buttonText = stringToWString(MyJson::get(root,"attackButtonText").asString());

	auto button = device->getGUIEnvironment()->addButton(buttonRect, 0, GUI_ID_ATTACK, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the defend button
	points = MyJson::get(root,"defendButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"defendButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, GUI_ID_DEFEND, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the skills button
	points = MyJson::get(root,"skillsButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"skillsButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, GUI_ID_SKILLS, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the items button
	points = MyJson::get(root,"itemsButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"itemsButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, GUI_ID_ITEMS, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the escape button
	points = MyJson::get(root,"escapeButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"escapeButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, GUI_ID_ESCAPE, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// target selection buttons
	// TODO generate these based on the number of enemies
	// TODO don't hardcode these parameters
	// TODO make some sort of target selection marker and use that instead of buttons
	button = device->getGUIEnvironment()->addButton(rect<s32>(420, 50, 548, 80), 0, GUI_ID_TARGET_1,
		enemyUnits[0]->name.c_str());
	button->setVisible(false);
	targetSelectButtons.push_back(button);

	button = device->getGUIEnvironment()->addButton(rect<s32>(650, 50, 778, 80), 0, GUI_ID_TARGET_2,
		enemyUnits[1]->name.c_str());
	button->setVisible(false);
	targetSelectButtons.push_back(button);

	button = device->getGUIEnvironment()->addButton(rect<s32>(870, 50, 998, 80), 0, GUI_ID_TARGET_3,
		enemyUnits[2]->name.c_str());
	button->setVisible(false);
	targetSelectButtons.push_back(button);

}

void BattleState::initializeUnitInfo()
{
	// for convenience
	using namespace irr;
	using namespace core;
	using namespace io;
	auto *gui = device->getGUIEnvironment();
	auto *driver = device->getVideoDriver();

	// load the json file
	// TODO don't hardcode the json path
	std::ifstream file("media/battle.json", std::ifstream::binary);
	Json::Value root;
	file >> root;
	file.close();

	// TODO don't hardcode these positions
	// and make sure to put this in a for loop once they're no longer hard coded
	playerUnits[0]->hpText = gui->addStaticText((std::to_wstring(playerUnits[0]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(300,530,450,600));
	playerUnits[1]->hpText = gui->addStaticText((std::to_wstring(playerUnits[1]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(450,530,550,600));
	playerUnits[2]->hpText = gui->addStaticText((std::to_wstring(playerUnits[2]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(600,530,700,600));
	playerUnits[3]->hpText = gui->addStaticText((std::to_wstring(playerUnits[3]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(750,530,850,600));
	playerUnits[4]->hpText = gui->addStaticText((std::to_wstring(playerUnits[4]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(900,530,1000,600));

	// add player unit portraits to the gui
	for (size_t i = 0; i < playerUnits.size(); i++)
	{
		playerUnits[i]->portrait = playerUnits[i]->portrait = driver->getTexture(("media/portrait" + std::to_string(i+1) + ".png").c_str());

		int x = MyJson::get(root, "portraitPositions")[i][0].asInt();
		int y = MyJson::get(root, "portraitPositions")[i][1].asInt();
		gui->addImage(playerUnits[i]->portrait, {x, y});
	}

	// add player unit full images to the gui
	for (size_t i = 0; i < playerUnits.size(); i++)
	{
		playerUnits[i]->portrait = playerUnits[i]->portrait = driver->getTexture(("media/portrait" + std::to_string(i+1) + ".png").c_str());

		int x = MyJson::get(root, "portraitPositions")[i][0].asInt();
		int y = MyJson::get(root, "portraitPositions")[i][1].asInt();
		gui->addImage(playerUnits[i]->portrait, {x, y});
	}

	// full body images
	playerUnits[0]->fullbodyImage = driver->getTexture("media/fullbody1.png");
	playerUnits[1]->fullbodyImage = driver->getTexture("media/fullbody2.png");
	playerUnits[2]->fullbodyImage = driver->getTexture("media/fullbody3.png");
	playerUnits[3]->fullbodyImage = driver->getTexture("media/fullbody4.png");
	playerUnits[4]->fullbodyImage = driver->getTexture("media/fullbody5.png");

	// enemy party setup
	// TODO get this from the previous gamestate
	enemyUnits.push_back(new EnemyUnit(L"Skeleton 1", Stats(5,3,3,3,1,30)));
	enemyUnits.push_back(new EnemyUnit(L"Skeleton 2", Stats(5,3,3,3,1,30)));
	enemyUnits.push_back(new EnemyUnit(L"Skeleton 3", Stats(5,3,3,3,1,30)));

	enemyUnits[0]->hpText = gui->addStaticText((std::to_wstring(enemyUnits[0]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(380,130,530,200));
	enemyUnits[1]->hpText = gui->addStaticText((std::to_wstring(enemyUnits[1]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(600,130,750,200));
	enemyUnits[2]->hpText = gui->addStaticText((std::to_wstring(enemyUnits[2]->currentStats.hp) + L"HP").c_str(),
		irr::core::rect<irr::s32>(820,130,970,200));

	enemyUnits[0]->fullbodyImage = driver->getTexture("media/skeleton.png");
	enemyUnits[1]->fullbodyImage = driver->getTexture("media/skeleton.png");
	enemyUnits[2]->fullbodyImage = driver->getTexture("media/skeleton.png");

	// set the player units hp text color
	for (auto u : playerUnits)
	{
		u->hpText->setOverrideColor(video::SColor(255,255,0,0));
	}

	// set the enemies hp text color
	for (auto u : enemyUnits)
	{
		u->hpText->setOverrideColor(video::SColor(255,255,0,0));
	}

	// skills menu
	skillsList = gui->addListBox(rect<s32>(100, 100, 300, 400), 0, -1, true);
	skillsList->setName(L"Skills");
	skillsList->setSelected(0);
	skillsList->setVisible(false);
	skillsList->setText(L"Skills");
}



void BattleState::update(float deltaTime)
{

// TEMPORARY ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO don't hardcode these positions and file paths
	// TODO don't hardcode all the text stuff
	// TODO get this from player party units and enemy units
	auto *driver = device->getVideoDriver();
	auto *gui = device->getGUIEnvironment();
	
	// update player unit text
	for (auto u : playerUnits)
	{
		u->hpText->setText((std::to_wstring(u->currentStats.hp) + L"HP").c_str());
	}

	// draw enemy unit images
	// TODO don't harcode parameters and put this in a loop
	driver->draw2DImage(enemyUnits[0]->fullbodyImage,{380,150}, irr::core::rect<irr::s32>(0,0,128,166), 0,
		irr::video::SColor(255,255,255,255), true);
	driver->draw2DImage(enemyUnits[1]->fullbodyImage, {600,150}, irr::core::rect<irr::s32>(0,0,128,166), 0,
		irr::video::SColor(255,255,255,255), true);
	driver->draw2DImage(enemyUnits[2]->fullbodyImage, {820,150}, irr::core::rect<irr::s32>(0,0,128,166), 0,
		irr::video::SColor(255,255,255,255), true);

	// update enemy unit text
	for (auto u : enemyUnits)
	{
		u->hpText->setText((std::to_wstring(u->currentStats.hp) + L"HP").c_str());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	updateState();
}

void BattleState::executeNextAction()
{
	actionText->setText(actions.front().getText().c_str());

	actions.front().execute();
	actions.pop();
}

void BattleState::undo()
{
	// TODO
}

void BattleState::updateState()
{
	//	TODO I can refactor this a lot i think. We don't need to be setting visibility and all that stuff every update.

	using namespace irr;
	using namespace core;
	using namespace io;

	// set everything invisible by default 
	for (auto b : targetSelectButtons)
	{
		b->setVisible(false);
	}
	for (auto b : mainButtons)
	{
		b->setVisible(false);
	}
	skillsList->setVisible(false);

	switch (state)
	{
	case State::Main:

		// TODO put unit name in the text here
		// TODO might be better not to hardcode the text so much
		actionText->setText((L"What will " + actingUnit->name + L" do?").c_str());

		// acting player units full image
		// TODO don't hardcode parameters
		// TODO would be more efficient to add this once to the gui and toggle its visibility
		// but we'll still have to udate it everytime the image changes
		device->getVideoDriver()->draw2DImage(actingUnit->fullbodyImage,{1000,150}, irr::core::rect<irr::s32>(0,0,300,571), 0,
			irr::video::SColor(255,255,255,255), true);

		for (auto b : mainButtons)
		{
			b->setVisible(true);
		}
		break;

	case State::SelectingTarget:

		// TODO put unit name in the text here
		// TODO might be better not to hardcode the text so much
		actionText->setText(L"Select your target?");
		
		for (auto b : targetSelectButtons)
		{
			b->setVisible(true);
		}
		break;

	case State::SelectingSkill:
		
		skillsList->setVisible(true);

		if (skillsList->getSelected() != -1)
		{
			selectedSkill = actingUnit->skills[skillsList->getSelected()];
			state = State::SelectingTarget;
		}
		
		break;

	case State::ExecutingActions:

		// TODO don't hardcode keybinds
		// if the mouse is down then we go to the next action
		if (eventReceiver->GetMouseState().LeftButtonDown)
		{
			if (!actions.empty())
			{
				executeNextAction();

				// TODO not sure if this is how we should handle this
				eventReceiver->mouse.LeftButtonDown = false;
			}
			else // were done with the executing actions state
			{
				refillNextActor();
				actingUnit = nextActor.top();
				nextActor.pop();
				state = State::Main;
			}
		}
		break;
	}

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

void BattleState::refillNextActor()
{
	for (auto u : playerUnits)
	{
		nextActor.push(u);
	}
	for (auto u : enemyUnits)
	{
		nextActor.push(u);
	}
}

void BattleState::addEnemyActions()
{
	while (!actingUnit->isFriendly())
	{
		// TODO use some sort of AI to select enemy actions and targets
		Action action = Action(Skills::basicAttack, *actingUnit);
		action.addTarget(*playerUnits[0]);
		actions.push(action);

		// exit the loop if next actor is empty here
		if (nextActor.empty())
		{
			actingUnit = nextActor.top();
			break;
		}
		actingUnit = nextActor.top();
		nextActor.pop();
	}
}

// MyEventReceiver stuff /////////////////////////////////////////////////////////////////////////////////////
BattleState::MyEventReceiver::MyEventReceiver(BattleState &parent, irr::IrrlichtDevice &device)
	: parent(parent), device(device)
{
	for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
	{
		keyIsDown[i] = false;
	}
}

bool BattleState::MyEventReceiver::OnEvent(const irr::SEvent &event)
{
	using namespace irr;
	using namespace io;
	using namespace gui;

	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		keyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
	}
	else if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		using namespace irr;
		// remember the mouse state
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch(event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				mouse.LeftButtonDown = true;
				break;

			case EMIE_LMOUSE_LEFT_UP:
				mouse.LeftButtonDown = false;
				break;

			case EMIE_RMOUSE_PRESSED_DOWN:
				mouse.RightButtonDown = true;
				break;

			case EMIE_RMOUSE_LEFT_UP:
				mouse.RightButtonDown = false;
				break;

			case EMIE_MOUSE_MOVED:
				mouse.Position.X = event.MouseInput.X;
				mouse.Position.Y = event.MouseInput.Y;
				break;

			default:
				break;
			}
		}
	}
	else if (event.EventType == EET_GUI_EVENT)
	{

		irr::s32 id = event.GUIEvent.Caller->getID();
		switch (event.GUIEvent.EventType)
		{
		case EGET_BUTTON_CLICKED:
			switch (id)
			{

			case GUI_ID_ATTACK:
				DEBUG_MODE(printf("attack button\n"));
				parent.selectedSkill = &Skills::basicAttack;
				parent.state = State::SelectingTarget;
				return true;						
												
			case GUI_ID_DEFEND:			
				DEBUG_MODE(printf("defend button\n"));
				return true;						
												
			case GUI_ID_SKILLS:			
				DEBUG_MODE(printf("skills button\n"));

				// TEMP this should be put somewhere else probably
				parent.skillsList->clear();
				for (auto s : parent.actingUnit->skills)
				{
					parent.skillsList->addItem(s->name.c_str());
				}

				parent.state = State::SelectingSkill;
				return true;

			case GUI_ID_ITEMS:
				DEBUG_MODE(printf("items button\n"));
				return true;

			case GUI_ID_ESCAPE:
				DEBUG_MODE(printf("escape button\n"));

				// TODO have a chance for escaping to fail and make it take a turn
				// TODO get this working without any state changing issues
				/*
				parent.nextState = GameStateType::DungeonState;
				parent.finished = false;
				*/

				return true;

			case GUI_ID_TARGET_1:
			{
				DEBUG_MODE(printf("target1 button\n"));
				TargetButton(1);
				return true;
			}

			case GUI_ID_TARGET_2:
			{
				DEBUG_MODE(printf("target2 button\n"));
				TargetButton(2);
				return true;
			}

			case GUI_ID_TARGET_3:
			{
				DEBUG_MODE(printf("target3 button\n"));
				TargetButton(3);
				return true;
			}

			default:
				return false;
			}
		}
	}

	return false;
}

void BattleState::MyEventReceiver::TargetButton(size_t buttonNumber)
{
	Action action(*parent.selectedSkill, *parent.actingUnit);
	action.addTarget(*parent.enemyUnits[buttonNumber - 1]);
	parent.actions.push(action);

	if (parent.nextActor.empty())
	{
		parent.state = State::ExecutingActions;
		parent.executeNextAction();
	}
	else
	{
		parent.actingUnit = parent.nextActor.top();
		parent.nextActor.pop();

		parent.addEnemyActions();
		if (parent.nextActor.empty())
		{
			parent.state = State::ExecutingActions;
			parent.executeNextAction();
		}
		else 
		{
			parent.state = State::Main;
		}
	}
}

bool BattleState::MyEventReceiver::IsKeyDown(irr::EKEY_CODE keyCode) const
{
	return keyIsDown[keyCode];
}

const BattleState::MyEventReceiver::SMouseState &BattleState::MyEventReceiver::GetMouseState() const
{
	return mouse;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// keybinds functions ///////////////////////////////////////////////////////////////////////////////////////
/*
BattleState::Keybinds::Keybinds()
{
	back = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN;
	next = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////