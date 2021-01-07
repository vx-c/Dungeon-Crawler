#pragma once

struct Stats
{
	// the base stats
	int strength = 1, intelligence = 1, faith = 1, vitality = 1, agility = 1; // TODO luck;

	int hp = 1;

	// TODO
	// secondary stats
//	int physicalPower, magicPower, healingPower, physicalDefence, magicalDefence, speed;

	// TODO
	// critical hit rate
	// dodge rate

	// TODO hp should be based on vitality
	Stats(int strength, int intelligence, int faith, int vitality, int agility, int hp)
		: strength(strength), intelligence(intelligence), faith(faith), agility (agility), hp(hp)
	{

	}

	Stats()
	{

	}

};
