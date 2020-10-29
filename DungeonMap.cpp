#include "DungeonMap.h"

#include <irrlicht.h>

#include <iostream>

#include <fstream>

#include <string>

#include "DebugMacro.h"

#include <SFML/Audio.hpp>

DungeonMap::DungeonMap(irr::IrrlichtDevice &device, const std::string &mapPath)
{
	using namespace irr;
	using namespace io;

	this->device = &device;

	loadDungeonFile(mapPath);
}

void DungeonMap::loadDungeonFile(const std::string &mapPath)
{
	std::ifstream myfile (mapPath);

	std::string token;
	
	// Get the information about the map
	// I would have used a switch statement but it doesn't support strings
	// I tried using stoi but it doesn't take that and I tried using an enum and then realized
	// I'll need to use if/else to convert the string to the enum value... so I guess this is how we're doing it
	while (myfile >> token)
	{
		if (token == "#comment")
		{
			while (myfile >> token)
			{
				if (token == "#comment")
				{
					break;
				}
			}
		}
		if (token == "width")
		{
			myfile >> width;
		}
		else if (token == "height")
		{
			myfile >> height;
		}
		else if (token == "scaling")
		{
			myfile >> scaling;
		}
		else if (token == "texturePath")
		{
			myfile >> texturePath;
		}
		else if (token == "textureExtension")
		{
			myfile >> textureExtension;
		}
		else if (token == "cameraHeight")
		{
			myfile >> cameraHeight;
		}
		else if (token == "bgm")
		{
			myfile >> bgmPath;
		}
		else if (token == "skyboxPath")
		{
			myfile >> skyboxPath;
		}
		else if (token == "skyboxExtension")
		{
			myfile >> skyboxExtension;
		}
		else if (token == "start")
		{
			int x, z;
			std::string tempFacing;
			myfile >> x >> z;
			myfile >> tempFacing;

			startPosition = irr::core::vector3df({x * scaling, cameraHeight * scaling, z * -scaling});
			
			if (tempFacing == "north")
			{
				startFacing = Directions::Value::North;
			}
			else if (tempFacing == "south")
			{
				startFacing = Directions::Value::South;
			}
			else if (tempFacing == "east")
			{
				startFacing = Directions::Value::East;
			}
			else if (tempFacing == "west")
			{
				startFacing = Directions::Value::West;
			}
		}
		// fill in the vectors based on the width and height
		else if (token == "walls")
		{
			for (int r = 0; r < height; r++)
			{
				walls.push_back(std::vector<int>());

				for (int c = 0; c < width; c++)
				{
					int tmp;

					myfile >> tmp;
					walls[r].push_back(tmp);
				}
			}
		}
		else if (token == "floor")
		{
			for (int r = 0; r < height; r++)
			{
				floor.push_back(std::vector<int>());

				for (int c = 0; c < width; c++)
				{
					int tmp;

					myfile >> tmp;
					floor[r].push_back(tmp);
				}
			}
		}
		else if (token == "ceiling")
		{
			for (int r = 0; r < height; r++)
			{
				ceiling.push_back(std::vector<int>());

				for (int c = 0; c < width; c++)
				{
					int tmp;

					myfile >> tmp;
					ceiling[r].push_back(tmp);
				}
			}
		}
	}

	
}

void DungeonMap::initializeScene()
{
	using namespace irr;
	using namespace scene;

	// NOTE/TODO this method assumes that the width and height are the same for all three layer vectors.
	// if that changes this needs to be adapted (though there any plans to change that really)
	for (int r = 0; r < width; r++)
	{
		for (int c = 0; c < height; c++)
		{
			if (walls[r][c] != 0)
			{
				ISceneNode * tmp = device->getSceneManager()->addCubeSceneNode();
				tmp->setPosition( {c * scaling, 0, r * -scaling} );
				tmp->setScale( {scaling/10, scaling/10, scaling/10} );
				tmp->setMaterialFlag(video::EMF_LIGHTING, false);

				core::stringw img = texturePath.c_str();
				img += walls[r][c];
				img += textureExtension.c_str();

				tmp->setMaterialTexture(0, device->getVideoDriver()->getTexture(img));
			}

			if (floor[r][c] != 0)
			{
				ISceneNode * tmp = device->getSceneManager()->addCubeSceneNode();
				tmp->setPosition( {c * scaling, -scaling, r * -scaling} );
				tmp->setScale( {scaling/10, scaling/10, scaling/10} );
				tmp->setMaterialFlag(video::EMF_LIGHTING, false);

				core::stringw img = texturePath.c_str();
				img += floor[r][c];
				img += textureExtension.c_str();

				tmp->setMaterialTexture(0, device->getVideoDriver()->getTexture(img));
			}

			if (ceiling[r][c] != 0)
			{
				ISceneNode * tmp = device->getSceneManager()->addCubeSceneNode();
				tmp->setPosition( {c * scaling, scaling, r * -scaling} );
				tmp->setScale( {scaling/10, scaling/10, scaling/10} );
				tmp->setMaterialFlag(video::EMF_LIGHTING, false);

				core::stringw img = texturePath.c_str();
				img += ceiling[r][c];
				img += textureExtension.c_str();

				tmp->setMaterialTexture(0, device->getVideoDriver()->getTexture(img));
			}
			
		}
	}

	// start playing the background music
	bgmBuffer;
	if (!bgmBuffer.loadFromFile("mymedia/bgm.wav"))
	{
		DEBUG_MODE(std::cerr << "failed to load bgm" << std::endl);
	}
	bgmSound.setBuffer(bgmBuffer);

	// TODO load bgm volume from a settings file
	bgmSound.setVolume(10);
	bgmSound.setLoop(true);
	bgmSound.play();

	// add the skybox
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

Directions::Value DungeonMap::getStartFacing()
{
	return startFacing;
}

float DungeonMap::getScaling()
{
	return scaling;
}

bool DungeonMap::isWallsPositionEmpty(irr::core::vector2df position)
{
	// TODO get rid of possible loss of data warning here
	irr::u32 row = round(-position.Y / scaling);
	irr::u32 col = round(position.X / scaling);

	if (walls[row][col] != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}