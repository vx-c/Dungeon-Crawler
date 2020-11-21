#include "PlayerUnit.h"

PlayerUnit::PlayerUnit()
{
	totalStats = Stats();
	currentStats = totalStats;

	friendly = true;
}

PlayerUnit::PlayerUnit(std::wstring name, Stats stats)
{
	totalStats = stats;
	currentStats = totalStats;
	this->name = name;
	friendly = true;
}
