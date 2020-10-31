#include "SoundManager.h"

#include <SFML/Audio.hpp>

#include "DebugMacro.h"

#include <fstream>

#include <map>

#include <uberswitch/uberswitch.hpp>

SoundManager::SoundManager()
{
	
}

void SoundManager::initialize(const std::string& soundsPath)
{
	std::ifstream myfile (soundsPath);

	std::string token;

	std::string path;

	float volume;

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

			case ("bgm"):
				myfile >> path >> volume;
				loadMusic(MusicType::bgm, path, volume);
				break;

			case ("move"):
				myfile >> path >> volume;
				loadSound(SoundType::move, path, volume);
				break;

			case ("turn"):
				myfile >> path >> volume;
				loadSound(SoundType::turn, path, volume);
				break;

			case ("wallBump"):
				myfile >> path >> volume;
				loadSound(SoundType::wallBump, path, volume);
				break;
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
