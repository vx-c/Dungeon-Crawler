#include "Directions.h"

#include <uberswitch/uberswitch.hpp>

#include <iostream>

#include "DebugMacro.h"

Directions::Directions()
{

}

Directions::Value Directions::stringToValue(std::string str)
{

	uberswitch(str)
	{
		case ("west"):
			return Value::West;

		case ("east"):
			return Value::East;

		case ("north"):
			return Value::North;

		case ("south"):
			return Value::South;

		default:
			std::cerr << "invalid string in Directions::stringToValue\n";
			return Value::North;
	}
}

Directions::Value Directions::getLeft(Value &facing)
{
	switch (facing)
	{
	case Value::East:
		return Value::North;

	case Value::North:
		return Value::West;

	case Value::West:
		return Value::South;

	case Value::South:
		return Value::East;
	}
}

Directions::Value Directions::getRight(Value &facing)
{
	switch (facing)
	{
	case Value::East:
		return Value::South;

	case Value::North:
		return Value::East;

	case Value::West:
		return Value::North;

	case Value::South:
		return Value::West;
	}
}

Directions::Value Directions::getReverse(Value &facing)
{
	switch (facing)
	{
	case Value::East:
		return Value::West;

	case Value::North:
		return Value::South;

	case Value::South:
		return Value::North;

	case Value::West:
		return Value::East;
	}
}

irr::core::vector3df Directions::getVector(Value &facing)
{
	switch (facing)
	{
	case Directions::Value::North:
		return {0,0,1};

	case Directions::Value::South:
		return {0,0,-1};

	case Directions::Value::East:
		return {1,0,0};

	case Directions::Value::West:
		return {-1,0,0};
	}
}