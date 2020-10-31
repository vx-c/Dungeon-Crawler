#include "SoundManager.h"

#include <SFML/Audio.hpp>

#include "DebugMacro.h"

#include <fstream>

#include <map>

SoundManager::SoundManager()
{
	
}

void SoundManager::initialize(const std::string& soundsPath)
{
	std::ifstream myfile (soundsPath);

	std::string token;

	// Get the information about the map
	// I would have used a switch statement but it doesn't support strings
	// I tried using stoi but it doesn't take that and I tried using an enum and then realized
	// I'll need to use if/else to convert the string to the enum value... so I guess this is how we're doing it
	while (myfile >> token)
	{
		// TODO this else if chain is ridiculous but I can't use a switch with a string (thanks C++)
		// we need to do something about this though because its disgusting
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
		else if (token == "bgm")
		{
			std::string path;
			float volume;
			myfile >> path >> volume;
			loadMusic(MusicType::bgm, path, volume);
		}
		else if (token == "move")
		{
			std::string path;
			float volume;
			myfile >> path >> volume;
			loadSound(SoundType::move, path, volume);
		}
		else if (token == "turn")
		{
			std::string path;
			float volume;
			myfile >> path >> volume;
			loadSound(SoundType::turn, path, volume);
		}
		else if (token == "wallBump")
		{
			std::string path;
			float volume;
			myfile >> path >> volume;
			loadSound(SoundType::wallBump, path, volume);
		}
	}
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
	printf("making sound\n");

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