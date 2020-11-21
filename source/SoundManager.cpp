#include "SoundManager.h"

#include <SFML/Audio.hpp>

#include "DebugMacro.h"

#include <fstream>

#include <map>

#include <json/json.h>

#include "MyJson.h"

#include "DebugMacro.h"

SoundManager::SoundManager()
{
	
}

void SoundManager::initialize(const std::string& soundsPath)
{
	std::ifstream file(soundsPath, std::ifstream::binary);

	Json::Value root;

	file >> root;

	loadMusic(MusicType::Bgm, root["bgm"].asString(), root["bgmVolume"].asFloat());

	DEBUG_MODE(MyJson::checkValid(root, "moveSound"));
	DEBUG_MODE(MyJson::checkValid(root, "moveVolume"));
	loadSound(SoundType::Move, root["moveSound"].asString(), root["moveVolume"].asFloat());
	
	DEBUG_MODE(MyJson::checkValid(root, "turnSound"));
	DEBUG_MODE(MyJson::checkValid(root, "turnVolume"));
	loadSound(SoundType::Turn, root["turnSound"].asString(), root["turnVolume"].asFloat());

	DEBUG_MODE(MyJson::checkValid(root, "wallBumpSound"));
	DEBUG_MODE(MyJson::checkValid(root, "wallBumpVolume"));
	loadSound(SoundType::WallBump, root["wallBumpSound"].asString(), root["wallBumpVolume"].asFloat());
}


void SoundManager::loadSound(SoundType sound, std::string path, float volume)
{
	sf::SoundBuffer buffer;

	if (!buffer.loadFromFile(path))
	{
		// TODO remove the print
		printf("sound failed to load\n");
	}

	buffers[sound] = buffer;
	soundVolumes[sound] = volume;
}

void SoundManager::loadMusic(MusicType music, std::string path, float volume)
{
	musics[music] = new sf::Music();

	if (!musics[music]->openFromFile(path))
	{
		// TODO remove the print
		printf("music failed to load\n");
	}
	musics[music]->setLoop(true);
	musics[music]->setVolume(volume);
}

sf::Sound SoundManager::makeSound(SoundType sound)
{
	// TODO deal with out of bounds exception
	sounds.push(sf::Sound());

	sf::Sound &out = sounds.back();
	out.setBuffer(buffers[sound]);
	out.setVolume(soundVolumes[sound]);
	out.play();

	return out;
}

sf::Music& SoundManager::playMusic(MusicType music)
{
	printf("playing music\n");

	musics[music]->play();

	return *musics[music];
}

void SoundManager::clear()
{
	if (!sounds.empty())
	{
		if (sounds.front().getStatus() == sf::Sound::Status::Stopped)
		{
			sounds.pop();
		}
	}
}

void SoundManager::cleanup()
{
	for (auto it = musics.begin(); it != musics.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}