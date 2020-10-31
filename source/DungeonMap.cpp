#include "DungeonMap.h"

#include <irrlicht.h>

#include <iostream>

#include <fstream>

#include <string>

#include "DebugMacro.h"

#include <SFML/Audio.hpp>

#include <uberswitch/uberswitch.hpp>

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
	
	// Thank you falmagn for making uberswitch
	// C++ really should have some sort of switch for strings.
	// Not sure how you're supposed to do file parsing like this without either a switch or a massive else if chain.
	while (myfile >> token)
	{
		uberswitch(token)
		{
			case ("#comment"):
				while (myfile >> token)
				{
					if (token == "#comment")
					{
						break;
					}
				}
				break;

			case ("width"):
				myfile >> width;
				break;

			case ("height"):
				myfile >> height;
				break;

			case("scaling"):
				myfile >> scaling;
				break;
			
			case ("texturePath"):
				myfile >> texturePath;
				break;

			case ("textureExtension"):
				myfile >> textureExtension;
				break;
			
			case ("cameraHeight"):
				myfile >> cameraHeight;
				break;

			case ("skyboxPath"):
				myfile >> skyboxPath;
				break;
		
			case ("skyboxExtension"):
				myfile >> skyboxExtension;
				break;
			
			case ("turnSpeed"):
				myfile >> turnSpeed;
				break;
			
			case ("moveSpeed"):
				myfile >> moveSpeed;
				break;

			case ("wallBumpSpeed"):
				myfile >> wallBumpSpeed;
				break;
			
			case ("wallBumpDistance"):
				myfile >> wallBumpDistance;
				break;
			
			case ("start"):
				int x, z;

				myfile >> x >> z;
				myfile >> token; // token is now the facing direction

				startPosition = irr::core::vector3df({x * scaling, cameraHeight * scaling, z * -scaling});

				if (token == "north")
				{
					startFacing = Directions::Value::North;
				}
				else if (token == "south")
				{
					startFacing = Directions::Value::South;
				}
				else if (token == "east")
				{
					startFacing = Directions::Value::East;
				}
				else if (token == "west")
				{
					startFacing = Directions::Value::West;
				}
				break;

			// fill in the vectors based on the width and height
			case ("walls"):
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
				break;

			case ("floor"):
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
				break;

			case ("ceiling"):
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
				break;

			default:
				break;
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
