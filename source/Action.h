#pragma once

#include "Unit.h"

#include <vector>

#include <iostream>

#include "Skills.h"

// An action used in battle
class Action
{
	std::vector<Unit *> targets;

	Unit &actingUnit;

	// functions for each different actions executions

	void executeBasicAttack();

	void executeFirewave();

	void executeFireball();

	void executeCleave();

	void executeDoNothing();

public:
	Action(const Skill &skill, Unit &actingUnit);

	// the skill
	const Skill &skill;

	// execute the action
	void execute();

	// add to targets
	void addTargets(std::vector<Unit> &targets);

	// add the target
	void addTarget(Unit &target);

	std::wstring getText();

};

