#pragma once

#include <iostream>
#include <vector>

#include <irrlicht.h>

#include "Directions.h"
#include <SFML/Audio.hpp>

// the map of the dungeon
class DungeonMap
{

	irr::IrrlichtDevice *device;

	// 2d vectors for each layer
	std::vector<std::vector<int>> walls;
	std::vector<std::vector<int>> floor;
	std::vector<std::vector<int>> ceiling;


	int width, height;
	float scaling, cameraHeight;
	std::string texturePath, textureExtension, bgmPath, skyboxPath, skyboxExtension;
	irr::core::vector3df startPosition;
	Directions::Value startFacing;
	sf::Sound bgmSound;
	sf::SoundBuffer bgmBuffer;


	// parse the dungeon file and setup the walls, floor, and ceiling vectors
	void loadDungeonFile(const std::string &mapPath);

public:
	DungeonMap(irr::IrrlichtDevice &device, const std::string &mapPath);

	// initialize the scene, add nodes to the scenemanager
	void initializeScene();

	// get the start position of the player
	irr::core::vector3df getStartPosition();

	// get the direction the player starts facing
	Directions::Value getStartFacing();

	// get the scaling of the dungeon
	float getScaling();

	// checks if a position in the walls grid is empty 
	bool isWallsPositionEmpty(irr::core::vector2df position);
};

