#include "Unit.h"

#include "DebugMacro.h"

Unit::Unit()
	: friendly(false)
{
	totalStats = Stats();
	currentStats = totalStats;
	name = L"";

	fullbodyImage = nullptr;
	hpText = nullptr;
        
        dead = true;
}

Unit::Unit(std::wstring name, Stats stats)
	: name(name), totalStats(stats), friendly(false)
{
	currentStats = totalStats;

	fullbodyImage = nullptr;
	hpText = nullptr;

        dead = true;
}

Unit::~Unit()
{
	// TODO
	// I think the lifespan of these is managed by the gui but i've added this todo just in case
	/*
	delete hpText;
	hpText = nullptr;
	delete fullbodyImage;
	fullbodyImage = nullptr;
	*/
}

bool Unit::isFriendly()
{
	return friendly;
}

void Unit::addSkill(Skill &skill)
{
	for (size_t i = 0; i < skills.size(); ++i)
	{
		if (skills[i]->id == skill.id)
		{
			DEBUG_MODE(std::cerr << "addskill error skill already in vector" << std::endl;);
			return;
		}
	}
	skills.push_back(&skill);
}
