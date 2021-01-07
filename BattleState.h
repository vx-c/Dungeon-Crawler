#pragma once
#include "GameState.h"

#include "Unit.h"

#include <vector>

#include <queue>

#include "Action.h"

#include "SoundManager.h"

#include "EnemyUnit.h"
#include "PlayerUnit.h"

#include <stack>

// the state used for battles in the game
class BattleState :
    public GameState
{
	// IDs for events
	enum
	{
		GUI_ID_ATTACK,
		GUI_ID_DEFEND,
		GUI_ID_SKILLS,
		GUI_ID_ITEMS,
		GUI_ID_ESCAPE,
		GUI_ID_TARGET_1,
		GUI_ID_TARGET_2,
		GUI_ID_TARGET_3,
		GUI_ID_ENDTURN,
		GUI_ID_UNDO
	};

	enum class State
	{
		Main, 
		SelectingTarget, 
		ExecutingActions, 
		ConfirmEndTurn,
		SelectingSkill
	};

	// the event receiver class for this gamestate
	class MyEventReceiver : public irr::IEventReceiver
	{
		irr::IrrlichtDevice &device;

		BattleState &parent;

		// we use this array to keep track of which keys are down and which keys are up
		bool keyIsDown[irr::KEY_KEY_CODES_COUNT];

		void TargetButton(size_t i);

	public:
		// stores mouse information
		struct SMouseState
		{
			irr::core::position2di Position;
			bool LeftButtonDown, RightButtonDown;
			SMouseState() : LeftButtonDown(false), RightButtonDown(false) { };
		};

		SMouseState mouse;

		MyEventReceiver(BattleState &parent, irr::IrrlichtDevice &device);

		// called when an event occurs
		virtual bool OnEvent(const irr::SEvent &event);

		// used to tell if a key is being held down
		virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const;

		// get the mouse state
		const SMouseState &GetMouseState() const;
	};

//	Keybinds keybinds;

	MyEventReceiver *eventReceiver;

	State state;
        State lastState;

        // sound
        SoundManager &soundManager;

	// gui stuff
	std::vector<irr::gui::IGUIButton*> mainButtons;
	std::vector<irr::gui::IGUIButton*> targetSelectButtons;
	irr::gui::IGUIStaticText *actionText;
	irr::gui::IGUIListBox *skillsList;

	// action related
        std::stack<Unit *> lastActors; // keeps track of the last actors in order 
        Unit *actingUnit;
	std::deque<Action> actionQueue;
	std::priority_queue<Unit*, std::vector<Unit*>, Unit::CompareSpeed> nextActor;

	// TODO not sure if we should copy
	Skill *selectedSkill;

	// parties
	std::vector<PlayerUnit*> playerUnits;
	std::vector<EnemyUnit> enemyUnits;

	// for updating the different states, call it once each update
	void updateStates();

	// add all of the actions for enemies at the front of the nextActor queue
	void addEnemyActions();

	// refills the next actor queue
	void refillNextActor();

	// undo, behaviour depends on the current state
	void undo();

	// add the buttons to the scene
	void initializeButtons();

	// initialize and add unit information to the scene
	void initializeUnitInfo();

        // updates the state member
        void changeState(State newState);
    
        // updates enemy unit gui objects
        void updateEnemyGui();

        // check if the player has won the battle
        void checkVictory();

        // check if the player has lost the battle
        void checkDefeat();

public:

	BattleState(irr::IrrlichtDevice &device, SoundManager &soundManager, 
                std::vector<PlayerUnit> *party, std::vector<EnemyUnit> enemies);
	~BattleState();

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

	// execute the next action
	void executeNextAction();

};

