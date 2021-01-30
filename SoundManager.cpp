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
        using namespace MyJson;    

	std::ifstream file(soundsPath, std::ifstream::binary);

	Json::Value root;

	file >> root;
        
	loadMusic(MusicType::Bgm, get(root, "bgm").asString(), get(root, "bgmVolume").asFloat());

        loadMusic(MusicType::Battle, get(root,"battleMusic").asString(),
                get(root, "battleMusicVolume").asFloat());

	loadSound(SoundType::Move, get(root, "moveSound").asString(), 
                get(root, "moveVolume").asFloat());

	loadSound(SoundType::Turn, get(root, "turnSound").asString(), 
                get(root, "turnVolume").asFloat());

	loadSound(SoundType::WallBump, get(root, "wallBumpSound").asString(), 
                get(root, "wallBumpVolume").asFloat());
}


void SoundManager::loadSound(SoundType sound, std::string path, float volume)
{
	sf::SoundBuffer buffer;

	if (!buffer.loadFromFile(path))
	{
		DEBUG_MODE(printf("sound failed to load\n"));
                return;
	}

	buffers[sound] = buffer;
	soundVolumes[sound] = volume;
}

void SoundManager::loadMusic(MusicType music, std::string path, float volume)
{
	musics[music] = new sf::Music();

	if (!musics[music]->openFromFile(path))
	{
		DEBUG_MODE(printf("music failed to load\n"));
                return;
	}
	musics[music]->setLoop(true);
	musics[music]->setVolume(volume);
}

sf::Sound& SoundManager::makeSound(SoundType sound)
{
	sounds.push(sf::Sound());

	sf::Sound &out = sounds.back();
	out.setBuffer(buffers[sound]);
	out.setVolume(soundVolumes[sound]);
	out.play();

	return out;
}

sf::Music& SoundManager::playMusic(MusicType music)
{
	DEBUG_MODE(printf("playing music\n"));

	musics[music]->play();
        
	return *musics[music];
}

void SoundManager::stopMusic(MusicType music)
{
	musics[music]->stop();
        
	return;
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
