#include "EnemyUnit.h"


EnemyUnit::EnemyUnit()
{
	totalStats = Stats();
	currentStats = totalStats;

	friendly = false;
}

EnemyUnit::EnemyUnit(std::wstring name, Stats stats)
{
	totalStats = stats;
	currentStats = totalStats;
	this->name = name;
	friendly = false;
}

Action EnemyUnit::getNextAction()
{

}