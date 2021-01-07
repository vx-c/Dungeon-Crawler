#pragma once

#include <string>

struct Skill
{
	enum class ID
	{
		BasicAttack,
		Cleave,
		Fireball,
		Firewave,
		DoNothing
	};

	enum class TargetingID
	{
		Single,
		All
	};

	std::wstring name;
	std::wstring text;

	ID id; 
	TargetingID targeting;

	Skill(ID id, TargetingID targeting, std::wstring name, std::wstring text);

};

namespace Skills
{
	extern Skill basicAttack;
	extern Skill cleave;
	extern Skill fireball;
	extern Skill firewave;
	extern Skill doNothing;
}
