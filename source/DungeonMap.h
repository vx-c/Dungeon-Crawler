#pragma once

#include <iostream>
#include <vector>

#include <irrlicht.h>

#include "Directions.h"
#include <SFML/Audio.hpp>

#include "SoundManager.h"

// the map of the dungeon
class DungeonMap
{
	irr::IrrlichtDevice *device;

	// 2d vectors for each layer
	std::vector<std::vector<int>> walls;
	std::vector<std::vector<int>> floor;
	std::vector<std::vector<int>> ceiling;

	// coordinate related
	irr::core::vector3df startPosition;
	irr::core::vector3df camStartPosition;
	Directions::Value startFacing;
	int width, height;
	float cameraHeight;
	irr::core::vector3df scaling;

	// player action speeds
	float turnSpeed, moveSpeed, wallBumpSpeed;
	float wallBumpDistance;

	// texture paths
	std::string texturePath, textureExtension, skyboxPath, skyboxExtension;

	// parse the dungeon file and setup the walls, floor, and ceiling vectors
	void loadDungeonFile(const std::string &mapPath);

public:
	DungeonMap(irr::IrrlichtDevice &device, const std::string &mapPath);

	// initialize the scene, add nodes to the scenemanager
	void initializeScene();

	// get the start position of the player
	irr::core::vector3df getStartPosition();

	// get the start position of the camera
	irr::core::vector3df getCamStartPosition();

	// get the direction the player starts facing
	Directions::Value getStartFacing();

	// get the scaling of the dungeon
	irr::core::vector3df getScaling();

	// get the camera position for a given point on the map
	irr::core::vector3df getPosition(int x, int y);

	// checks if a position in the walls grid is empty 
	bool isWallsPositionEmpty(irr::core::vector2df position);

	// get the move speed
	float getMoveSpeed();

	// get the turn speed
	float getTurnSpeed();

	// get the wallbump distance
	float getBumpDistance();

	// get the wallbump speed
	float getBumpSpeed();
};

