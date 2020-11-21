#include "DungeonMap.h"

#include <irrlicht.h>

#include <iostream>

#include <fstream>

#include <string>

#include "DebugMacro.h"

#include <SFML/Audio.hpp>

#include <json/json.h>

#include "MyJson.h"

DungeonMap::DungeonMap(irr::IrrlichtDevice &device, const std::string &mapPath)
{
	using namespace irr;
	using namespace io;

	this->device = &device;

	loadDungeonFile(mapPath);
}


void DungeonMap::loadDungeonFile(const std::string &mapPath)
{
	// TODO if you feel like doing something tedious and sort of unnecessary
	// you can replace these Myjson::checkValid calls with MyJson::get

	std::ifstream file(mapPath, std::ifstream::binary);
	Json::Value root;
	file >> root;
	file.close();

	width = MyJson::get(root, "width").asInt();
	height = MyJson::get(root, "height").asInt();

	float scalingXZ = MyJson::get(root, "scalingXZ").asFloat();
	float scalingY = MyJson::get(root, "scalingY").asFloat();
	scaling = {scalingXZ, scalingY, scalingXZ};
	cameraHeight = MyJson::get(root, "cameraHeight").asFloat();

	texturePath = MyJson::get(root, "texturePath").asString();
	textureExtension = MyJson::get(root, "textureExtension").asString();

	skyboxPath = MyJson::get(root, "skyboxPath").asString();
	skyboxExtension = MyJson::get(root, "skyboxExtension").asString();

	turnSpeed = MyJson::get(root, "turnSpeed").asFloat();
	moveSpeed = MyJson::get(root, "moveSpeed").asFloat();

	wallBumpSpeed = MyJson::get(root, "wallBumpSpeed").asFloat();
	wallBumpDistance = MyJson::get(root, "wallBumpDistance").asFloat();

	float startPositionX = MyJson::get(root, "startPositionX").asFloat();
	float startPositionY = MyJson::get(root, "startPositionY").asFloat();

	startPosition = irr::core::vector3df({startPositionX, 0, startPositionY});
	camStartPosition = {scaling.X * startPositionX, 0, -scaling.Z * startPositionY};

	Directions directions;
	startFacing = directions.stringToValue(MyJson::get(root, "startFacing").asString());
		
	// fill in the vectors based on the width and height
	DEBUG_MODE(MyJson::checkValid(root, "floor"));
	for (int r = 0, index = 0; r < height; r++)
	{
		floor.push_back(std::vector<int>());

		for (int c = 0; c < width; c++)
		{
			floor[r].push_back(root["floor"][index].asInt());
			index++;
		}
	}

	DEBUG_MODE(MyJson::checkValid(root, "ceiling"));
	for (int r = 0, index = 0; r < height; r++)
	{
		ceiling.push_back(std::vector<int>());

		for (int c = 0; c < width; c++)
		{
			ceiling[r].push_back(root["ceiling"][index].asInt());
			index++;
		}
	}

	DEBUG_MODE(MyJson::checkValid(root, "walls"));
	for (int r = 0, index = 0; r < height; r++)
	{
		walls.push_back(std::vector<int>());

		for (int c = 0; c < width; c++)
		{
			walls[r].push_back(root["walls"][index].asInt());
			index++;
		}
	}

}


void DungeonMap::initializeScene()
{
	using namespace irr;
	using namespace scene;

	// NOTE/TODO this method assumes that the width and height are the same for all three layer vectors.
	// if that changes this needs to be adapted (though there any plans to change that really)

	// make the scaling more adjustable and put it in the json

	for (int r = 0; r < width; r++)
	{
		for (int c = 0; c < height; c++)
		{
			if (walls[r][c] != 0)
			{
				ISceneNode * tmp = device->getSceneManager()->addCubeSceneNode();
				tmp->setPosition( {c * scaling.X, 0, r * -scaling.Z} );
				tmp->setScale( {scaling.X/10, 2 * scaling.Y/10, scaling.Z/10} );
				tmp->setMaterialFlag(video::EMF_LIGHTING, false);

				core::stringw img = texturePath.c_str();
				img += walls[r][c];
				img += textureExtension.c_str();

				tmp->setMaterialTexture(0, device->getVideoDriver()->getTexture(img));


				//temp
				// TODO figure out why textures are blurry at odd angles and distances
				tmp->getMaterial(0).TextureLayer[0].LODBias = 2.f;


			}

			if (floor[r][c] != 0)
			{
				ISceneNode * tmp = device->getSceneManager()->addCubeSceneNode();
				tmp->setPosition( {c * scaling.X, -scaling.Y, r * -scaling.Z} );
				tmp->setScale( {scaling.X/10, scaling.Y/10, scaling.Z/10} );
				tmp->setMaterialFlag(video::EMF_LIGHTING, false);

				core::stringw img = texturePath.c_str();
				img += floor[r][c];
				img += textureExtension.c_str();

				tmp->setMaterialTexture(0, device->getVideoDriver()->getTexture(img));

				//temp
				// TODO figure out why textures are blurry at odd angles and distances
				tmp->getMaterial(0).TextureLayer[0].LODBias = 2.f;
			}

			/* TEMP
			* // TODO put ceiling a bit higher
			if (ceiling[r][c] != 0)
			{
				ISceneNode * tmp = device->getSceneManager()->addCubeSceneNode();
				tmp->setPosition( {c * scaling.X, scaling.Y, r * -scaling.Z} );
				tmp->setScale( {scaling.X/10, scaling.Y/10, scaling.Z/10} );
				tmp->setMaterialFlag(video::EMF_LIGHTING, false);

				core::stringw img = texturePath.c_str();
				img += ceiling[r][c];
				img += textureExtension.c_str();

				tmp->setMaterialTexture(0, device->getVideoDriver()->getTexture(img));
			}
			*/
			
			
		}
	}

	// add the skybox
	// TODO im not sure if the "top left back front right left" stuff should be hardcoded
	// though its probably not worth working around in this case
	irr::scene::ISceneNode *skybox = device->getSceneManager()->addSkyBoxSceneNode(
		device->getVideoDriver()->getTexture((skyboxPath + "top" + skyboxExtension).c_str()),
		device->getVideoDriver()->getTexture((skyboxPath + "bottom" + skyboxExtension).c_str()),
		device->getVideoDriver()->getTexture((skyboxPath + "left" + skyboxExtension).c_str()),
		device->getVideoDriver()->getTexture((skyboxPath + "right" + skyboxExtension).c_str()),
		device->getVideoDriver()->getTexture((skyboxPath + "front" + skyboxExtension).c_str()),
		device->getVideoDriver()->getTexture((skyboxPath + "back" + skyboxExtension).c_str())
	);
}

irr::core::vector3df DungeonMap::getStartPosition()
{
	return startPosition;
}

irr::core::vector3df DungeonMap::getCamStartPosition()
{
	return camStartPosition;
}

Directions::Value DungeonMap::getStartFacing()
{
	return startFacing;
}

irr::core::vector3df DungeonMap::getScaling()
{
	return scaling;
}

bool DungeonMap::isWallsPositionEmpty(irr::core::vector2df position)
{
	irr::u32 row = irr::u32(round(-position.Y / scaling.X));
	irr::u32 col = irr::u32(round(position.X / scaling.Z));

	if (row >= walls.size() || col >= walls[0].size())
	{
		DEBUG_MODE(std::cerr << "out of bounds position in isWallsPositionEmpty\n");
		return false;
	}
	else
	{
		if (walls[row][col] != 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

irr::core::vector3df DungeonMap::getPosition(int x, int y)
{
	return {scaling.X * x, 0, -scaling.Z * y};
}

float DungeonMap::getTurnSpeed()
{
	return turnSpeed;
}

float DungeonMap::getMoveSpeed()
{
	return moveSpeed;
}

float DungeonMap::getBumpDistance()
{
	return wallBumpDistance;
}

float DungeonMap::getBumpSpeed()
{
	return wallBumpSpeed;
}