#pragma once

#include <irrlicht.h>

// loads and stores keybind information
struct Keybinds
{
	irr::EKEY_CODE moveForward, moveBackward, turnLeft, turnRight, exitGame; 

	Keybinds();
};

