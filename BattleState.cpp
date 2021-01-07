#include "BattleState.h"

#include "DebugMacro.h"

#include "Unit.h"

#include <iostream>

#include <string>

#include "PlayerUnit.h"
#include "EnemyUnit.h"

#include <fstream>

#include <wchar.h>

#include "StringConversion.h"

#include "MyJson.h"

BattleState::BattleState(irr::IrrlichtDevice &device, SoundManager &soundManager, 
        std::vector<PlayerUnit> *party, std::vector<EnemyUnit> enemyUnits) 
        :       selectedSkill(&Skills::doNothing), enemyUnits(enemyUnits), 
        soundManager(soundManager)
{
	nextState = GameStateType::BattleState;
	finished = false;
	this->device = &device;

	state = State::Main;
        lastState = State::Main;

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
	//TODO implement totalReset based differences

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

        soundManager.playMusic(MusicType::Battle);

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
                irr::video::SColor(colors[0].asInt(), colors[1].asInt(), 
                    colors[2].asInt(), colors[3].asInt()));

	// add buttons
	// TODO center the buttons and make things look good

	// button images
	auto buttonImage = driver->getTexture(stringToWString(MyJson::get(root,
                        "attackButtonImage").asString()).c_str());
	auto buttonPressedImage = driver->getTexture(stringToWString(MyJson::get(root,
                        "attackButtonPressedImage").asString()).c_str());

	// setup the attack button
	auto points = MyJson::get(root,"attackButtonRect");
	rect<s32> buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), 
                points[2].asInt(), points[3].asInt());
	std::wstring buttonText = stringToWString(MyJson::get(root,"attackButtonText").asString());

	auto button = device->getGUIEnvironment()->addButton(buttonRect, 0, 
                GUI_ID_ATTACK, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the defend button
	points = MyJson::get(root,"defendButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), 
                points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"defendButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, 
                GUI_ID_DEFEND, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the skills button
	points = MyJson::get(root,"skillsButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), 
                points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"skillsButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, 
                GUI_ID_SKILLS, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the items button
	points = MyJson::get(root,"itemsButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), 
                points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"itemsButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, 
                GUI_ID_ITEMS, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// setup the escape button
	points = MyJson::get(root,"escapeButtonRect");
	buttonRect = rect<s32>(points[0].asInt(), points[1].asInt(), 
                points[2].asInt(), points[3].asInt());
	buttonText = stringToWString(MyJson::get(root,"escapeButtonText").asString());

	button = device->getGUIEnvironment()->addButton(buttonRect, 0, 
                GUI_ID_ESCAPE, buttonText.c_str());
	button->setImage(buttonImage);
	button->setPressedImage(buttonPressedImage);
	mainButtons.push_back(button);

	// target selection buttons
	// TODO generate these based on the number of enemies
	// TODO don't hardcode these parameters
	// TODO make some sort of target selection marker and use that instead of buttons
	button = device->getGUIEnvironment()->addButton(rect<s32>(420, 50, 548, 80), 
                0, GUI_ID_TARGET_1, enemyUnits[0].name.c_str());
	button->setVisible(false);
	targetSelectButtons.push_back(button);

	button = device->getGUIEnvironment()->addButton(rect<s32>(650, 50, 778, 80), 
                0, GUI_ID_TARGET_2, enemyUnits[1].name.c_str());
	button->setVisible(false);
	targetSelectButtons.push_back(button);

	button = device->getGUIEnvironment()->addButton(rect<s32>(870, 50, 998, 80), 
                0, GUI_ID_TARGET_3, enemyUnits[2].name.c_str());
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

        // TODO don't hardcode these paths
	// add player unit portraits to the gui
	for (size_t i = 0; i < playerUnits.size(); i++)
	{
		playerUnits[i]->portrait = playerUnits[i]->portrait = driver->getTexture(("media/player/portrait" + std::to_string(i+1) + ".png").c_str());

		int x = MyJson::get(root, "portraitPositions")[int(i)][int(0)].asInt();
		int y = MyJson::get(root, "portraitPositions")[int(i)][int(1)].asInt();
		gui->addImage(playerUnits[i]->portrait, {x, y});
	}

        // TODO don't hardcode these paths
	// add player unit full images to the gui
	for (size_t i = 0; i < playerUnits.size(); i++)
	{
		playerUnits[i]->portrait = playerUnits[i]->portrait = driver->getTexture(("media/player/portrait" + std::to_string(i+1) + ".png").c_str());

		int x = MyJson::get(root, "portraitPositions")[int(i)][int(0)].asInt();
		int y = MyJson::get(root, "portraitPositions")[int(i)][int(1)].asInt();
		gui->addImage(playerUnits[i]->portrait, {x, y});
	}

        // TODO don't hardcode these paths
	// full body images
	playerUnits[0]->fullbodyImage = driver->getTexture("media/player/fullbody1.png");
	playerUnits[1]->fullbodyImage = driver->getTexture("media/player/fullbody2.png");
	playerUnits[2]->fullbodyImage = driver->getTexture("media/player/fullbody3.png");
	playerUnits[3]->fullbodyImage = driver->getTexture("media/player/fullbody4.png");
	playerUnits[4]->fullbodyImage = driver->getTexture("media/player/fullbody5.png");

        updateEnemyGui();

	for (auto u : playerUnits)
	{
                if (!u->dead)
                {
	        	u->hpText->setOverrideColor(video::SColor(255,255,0,0));
                }

                gui->addStaticText(u->name, 10,10, 200, 200);
        }


	// skills menu
	skillsList = gui->addListBox(rect<s32>(100, 100, 300, 400), 0, -1, true);
	skillsList->setName(L"Skills");
	skillsList->setSelected(0);
	skillsList->setVisible(false);
	skillsList->setText(L"Skills");
}

void BattleState::updateEnemyGui()
{
 	auto *gui = device->getGUIEnvironment();
	auto *driver = device->getVideoDriver();
       
        // TODO don't hardcode this
        std::vector<irr::core::rect<irr::s32>> rects;
        rects.push_back(irr::core::rect<irr::s32>(380,130,530,200));
        rects.push_back(irr::core::rect<irr::s32>(600,130,750,200));
        rects.push_back(irr::core::rect<irr::s32>(820,130,970,200));
        
        // TODO don't hardcode this stuff
        int x = 380;
        int xinc = 220;

        for (size_t i = 0; i < enemyUnits.size(); ++i)
        {
            // TODO we could skip this check if we put this in an initialize function, optimize
            if (enemyUnits[i].hpText == nullptr)
            {
                enemyUnits[i].hpText = gui->addStaticText(
                     (std::to_wstring(enemyUnits[i].currentStats.hp) + L"HP").c_str(), rects[i]);
                enemyUnits[i].hpText->setOverrideColor(irr::video::SColor(255,255,0,0));    
            }
                    
            // TODO don't hardcode the enemy path, replace skeleton with some variable
            // it would be a good idea to load the different enemy types and their data
            // from a resource file which would specify the path to their texture.
            // TODO we can skip this nullptr check if we put this in an init function, optimize
            if (enemyUnits[i].fullbodyImage == nullptr)
            {
                enemyUnits[i].fullbodyImage = driver->getTexture("media/monsters/skeleton.png");
            }

            if (!enemyUnits[i].dead)
            {   
                driver->draw2DImage(enemyUnits[i].fullbodyImage, {x + i*xinc, 150}, 
                    irr::core::rect<irr::s32>(0,0,101,200), 0, 
                    irr::video::SColor(255,255,255,255), true);
		
                // hp text
                enemyUnits[i].hpText->setText((std::to_wstring(enemyUnits[i].currentStats.hp) 
                    + L"HP").c_str());
            }
        }
}

void BattleState::changeState(State newState)
{
    lastState = state;
    state = newState;
}

void BattleState::update(float deltaTime)
{
// TEMPORARY /////////////////////////////////////////////////////////////////////////////////////////
	// TODO don't hardcode these positions and file paths
	// TODO don't hardcode all the text stuff
	// TODO get this from player party units and enemy units
        // TODO we shouldn't need to do some of this stuff every single frame, optimize
	auto *driver = device->getVideoDriver();
	auto *gui = device->getGUIEnvironment();
	
	// update player unit text
	for (auto u : playerUnits)
	{
		u->hpText->setText((std::to_wstring(u->currentStats.hp) + L"HP").c_str());
	}
       
        // TODO we should seperate our gui updates between things that are updated every frame
        // and things that are updated sometimes for efficiency
        updateEnemyGui();

//////////////////////////////////////////////////////////////////////////////////////////////////////

	updateStates();
}

void BattleState::executeNextAction()
{
        Action *action = &actionQueue.front();

        // skip dead unit's actions
        while (action->actingUnit.dead)
        {
            std::wcout << L"skipping " << action->actingUnit.name << std::endl;
            actionQueue.pop_front();
            
            if (actionQueue.size() > 0)
            {
                action = &actionQueue.front();
            }
            else
            {
                return;
            }
        }
        
        // retarget actions if the target has died
        if (action->skill.targeting == Skill::TargetingID::Single)
        {
            if (action->targets[0]->dead)
            {
                if (action->actingUnit.isFriendly())
                    {
                    for (size_t i = 0; i < enemyUnits.size(); i++)
                    {
                        if (!enemyUnits[i].dead)
                        {
                            std::wcout << "here " << enemyUnits[i].name << std::endl;
                        
                            action->clearTargets();
                            action->addTarget(&enemyUnits[i]);
                            break;
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < playerUnits.size(); i++)
                    {
                        if (!playerUnits[i]->dead)
                        {
                            std::wcout << "here " << playerUnits[i]->name << std::endl;
                        
                            action->clearTargets();
                            action->addTarget(playerUnits[i]);
                            break;
                        }
                    }
                }
            }
        }
        // TODO retarget actions for multi targeting

        action->execute();
        actionText->setText(action->getText().c_str());

        // check if any of the targets died
        for (size_t i = 0; i < action->targets.size(); ++i)
        {
            if (action->targets[i]->currentStats.hp < 1)
            {
                (action->targets)[i]->dead = true;
                (action->targets)[i]->hpText->setVisible(false);
                
                /*
                if (action->actingUnit.isFriendly())
                {
                    checkVictory();
                }
                else
                {
                    checkDefeat();
                }
                */
            }
        }
        
        actionQueue.pop_front();
}

void BattleState::checkVictory()
{
    // the player wins if all the enemies are dead
    bool victory = true;
    for (auto u : enemyUnits)
        {
            if (!u.dead)
            {
                victory = false;
                break;
            }
        }
        if (victory)
        {
            nextState = GameStateType::DungeonState;
	    finished = true;
            soundManager.stopMusic(MusicType::Battle);
        }
}

void BattleState::checkDefeat()
{
    // the player loses if all the player units are dead
    bool defeat = true;
    for (auto u : playerUnits)
        {
            if (!u->dead)
            {
                defeat = false;
                break;
            }
        }
        if (defeat)
        {
            // TODO implement losing screen and correctly handle this
            nextState = GameStateType::MainMenuState;
	    finished = true;
            for (size_t i = 0; i < playerUnits.size(); ++i)
            {
                playerUnits[i]->currentStats = playerUnits[i]->totalStats;
                playerUnits[i]->dead = false;
                soundManager.stopMusic(MusicType::Battle);
            }
        }
}

void BattleState::undo()
{
        switch (state) 
        {
            case State::Main:
                if (actionQueue.empty() || lastActors.empty())
                {
                    break;
                }

                actionQueue.pop_back();
                
                nextActor.push(actingUnit);

                actingUnit = lastActors.top();
                lastActors.pop();
                break;

            case State::SelectingTarget:
                if (lastState == State::SelectingSkill)
                {
                    skillsList->setSelected(-1);
                    changeState(State::SelectingSkill);
                }
                else
                {
                    changeState(State::Main);
                }
                break;

            case State::SelectingSkill:
                changeState(State::Main);
                break;

            default:
                break;
        }
}

void BattleState::updateStates()
{

	//	TODO I can refactor this a lot i think.
        //	We don't need to be setting visibility and all that stuff every update.

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
		// but we'll still have to update it everytime the image changes
		device->getVideoDriver()->draw2DImage(actingUnit->fullbodyImage,{1000,150}, 
                        irr::core::rect<irr::s32>(0,0,300,571), 0,
			irr::video::SColor(255,255,255,255), true);

		for (auto b : mainButtons)
		{
			b->setVisible(true);
		}

		if (eventReceiver->GetMouseState().RightButtonDown)
		{
			undo();
			eventReceiver->mouse.RightButtonDown = false;
                        break;
		}
		break;

	case State::SelectingTarget:

		// TODO put unit name in the text here
		// TODO might be better not to hardcode the text so much
		actionText->setText(L"Select your target?");

		for (size_t i = 0; i < targetSelectButtons.size(); ++i)
		{
                        if (!enemyUnits[i].dead)
                        {
			    targetSelectButtons[i]->setVisible(true);
                        }

		}

		if (eventReceiver->GetMouseState().RightButtonDown)
		{
			undo();
                        eventReceiver->mouse.RightButtonDown = false;
		}
                break;

	case State::SelectingSkill:
		
		skillsList->setVisible(true);

		if (skillsList->getSelected() != -1)
		{
			selectedSkill = actingUnit->skills[skillsList->getSelected()];
		        changeState(State::SelectingTarget);	
		}
		else if (eventReceiver->GetMouseState().RightButtonDown)
		{
			undo();
                        eventReceiver->mouse.RightButtonDown=false;
		}
		
		break;

	case State::ExecutingActions:
                // TODO don't hardcode keybinds
		// if the mouse is down then we go to the next action
		if (eventReceiver->GetMouseState().LeftButtonDown)
		{               
			checkDefeat();
                        checkVictory();

                        if (!actionQueue.empty())
			{
                                executeNextAction();

				// TODO not sure if this is how we should handle this
				eventReceiver->mouse.LeftButtonDown = false;
			}
			else // were done with the executing actions state
			{
				refillNextActor();
                                changeState(State::Main);
			}
		}
		else if (eventReceiver->GetMouseState().RightButtonDown)
		{
			undo();
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
                if (!u->dead)
                {
	            nextActor.push(u);
                }
        }
        
        lastActors = {};

	actingUnit = nextActor.top();
	nextActor.pop(); // don't forget this pop, otherwise there is an extra action
}


void BattleState::addEnemyActions()
{
	for (size_t i = 0; i < enemyUnits.size(); i++)
	{
                if (enemyUnits[i].dead)
                {
                    continue;
                }

		// TODO use some sort of ai algorithm to select enemy actions and targets
                std::wcout << "here " << enemyUnits[i].name << std::endl;
                
                Action action = Action(Skills::basicAttack, enemyUnits[i]);

                // randomly select target from  living player units
                std::vector<Unit*> possibleTargets = {};
                for (size_t i = 0; i < playerUnits.size(); ++i)
                {
                    if (!playerUnits[i]->dead)
                    {
                        possibleTargets.push_back(playerUnits[i]);
                    }
                }
                int target = rand() % (possibleTargets.size() - 1) + 0;

                std::cout << "adding enemy actions " << possibleTargets.size() << target << std::endl;
		action.addTarget(possibleTargets[target]);

		actionQueue.push_back(action);
		
		/*
		// place the enemy action in the correct place based on the enemies speed
		bool added = false;
		for (size_t i = 0; i < actionQueue.size(); ++i)
		{
			// TODO randomize
			if (actions[i].actingUnit.currentStats.agility < u->currentStats.agility)
			{
				actions.emplace(actions.begin() + i, action);
				added = true;
				break;
			}
		}
		if (!added)
		{
			actions.emplace(actions.end(), action);
		}
		*/

	}

	/* TEMP
	while (!actingUnit->isFriendly())
	{
		// TODO use some sort of AI to select enemy actions and targets
		Action action = Action(Skills::basicAttack, *actingUnit);
		action.addTarget(*playerUnits[0]);
		actions.push_back(action);

		// exit the loop if next actor is empty here
		if (nextActor.empty())
		{
			actingUnit = nextActor.top();
			break;
		}
		actingUnit = nextActor.top();
		nextActor.pop();
	}
	*/
}

// MyEventReceiver stuff /////////////////////////////////////////////////////////////////////////////
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
				parent.changeState(State::SelectingTarget);
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

				parent.changeState(State::SelectingSkill);
				return true;

			case GUI_ID_ITEMS:
				DEBUG_MODE(printf("items button\n"));
				return true;

			case GUI_ID_ESCAPE:
				DEBUG_MODE(printf("escape button\n"));

				// TODO have a chance for escaping to fail and make it take a turn
				// TODO get this working without any state changing issues
				
				parent.nextState = GameStateType::DungeonState;
				parent.finished = true;
			        parent.soundManager.stopMusic(MusicType::Battle);
				
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
	action.addTarget(&parent.enemyUnits[buttonNumber - 1]);
	parent.actionQueue.push_back(action);

	if (parent.nextActor.empty()) 
	{
		// the player has chosen all of their actions
		// add the enemy actions and then change the state
		parent.addEnemyActions();
		parent.changeState(State::ExecutingActions);
		parent.executeNextAction();
	}
	
	else
	{
		parent.lastActors.push(parent.actingUnit);
		parent.actingUnit = parent.nextActor.top();
		parent.nextActor.pop();
                parent.changeState(State::Main);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////

// keybinds functions ////////////////////////////////////////////////////////////////////////////////
/*
BattleState::Keybinds::Keybinds()
{
	back = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN;
	next = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
