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
	const Skill basicAttack(Skill::ID::BasicAttack, Skill::TargetingID::Single, L"BasicAttack", L"attacks");
	const Skill cleave(Skill::ID::Cleave, Skill::TargetingID::All, L"Cleave", L"cleaves");

	const Skill fireball(Skill::ID::Fireball, Skill::TargetingID::Single, L"Fireball", L"casts fireball on");
	const Skill firewave(Skill::ID::Firewave, Skill::TargetingID::All, L"Firewave", L"casts firewave on");
	
	const Skill doNothing(Skill::ID::DoNothing, Skill::TargetingID::Single, L"DoNothing", L"does nothing to");
}
