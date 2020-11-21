#include "Action.h"

#include <iostream>


Action::Action(const Skill &skill, Unit &actingUnit)
	: skill(skill), actingUnit(actingUnit)
{
	
}

void Action::addTargets(std::vector<Unit> &targets)
{
	for (auto &t : targets)
	{
		targets.push_back(t);
	}
}

void Action::addTarget(Unit &target)
{
	targets.push_back(&target);
}

std::wstring Action::getText()
{
	// TODO not sure if we should just get targets[0] like that
	// we may later want to get the text for each specific target so we might need to do something else

	return actingUnit.name + L" " + skill.text + L" " + targets[0]->name + L".";
}

void Action::execute()
{

	if (targets.size() < 1)
	{
		std::cerr << "error no action targets" << std::endl;
		return;
	}

	switch (skill.id)
	{
	case Skill::ID::BasicAttack:
		executeBasicAttack();
		break;

	case Skill::ID::Cleave:
		executeCleave();
		break;

	case Skill::ID::Fireball:
		executeFireball();
		break;

	case Skill::ID::Firewave:
		executeFirewave();
		break;

	case Skill::ID::DoNothing:
		executeDoNothing();
		break;

	default:
		std::cerr << "error no action name" << std::endl;
		return;
	}
}

void Action::executeBasicAttack()
{
	// TODO improve this damage formula

	for (size_t i = 0; i < targets.size(); ++i)
	{
		targets[i]->currentStats.hp -= actingUnit.currentStats.strength;
	}
}

void Action::executeFirewave()
{
	// TODO improve this damage formula

	for (size_t i = 0; i < targets.size(); ++i)
	{
		targets[i]->currentStats.hp -= actingUnit.currentStats.intelligence;
	}
}

void Action::executeFireball()
{
	// TODO improve this damage formula

	for (size_t i = 0; i < targets.size(); ++i)
	{
		targets[i]->currentStats.hp -= actingUnit.currentStats.intelligence;
	}
}

void Action::executeCleave()
{
	// TODO improve this damage formula

	for (size_t i = 0; i < targets.size(); ++i)
	{
		targets[i]->currentStats.hp -= actingUnit.currentStats.strength;
	}
}

void Action::executeDoNothing()
{
	return;
}