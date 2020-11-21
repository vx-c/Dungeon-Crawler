#pragma once

#include <map>
#include <iostream>

#include <SFML/Audio.hpp>

#include <queue>

enum class SoundType
{
	Move, Turn, WallBump
};

enum class MusicType
{
	Bgm
};

class SoundManager
{
	std::map<SoundType, sf::SoundBuffer> buffers;
	std::map<SoundType, float> soundVolumes;

	// we use this to keep the sounds from exiting scope while they are playing
	std::queue<sf::Sound> sounds;

	std::map<MusicType, sf::Music*> musics;

public:

	SoundManager();

	void initialize(const std::string &mapPath);

	// load the sound
	void loadSound(SoundType sound, std::string path, float volume = 100);

	// load the music
	void loadMusic(MusicType music, std::string path, float volume = 100);

	// make, play, and return an sf::sound object for the sound effect
	sf::Sound makeSound(SoundType sound);

	// play and return music
	sf::Music& playMusic(MusicType music);

	// clears finished sounds
	void clear();

	// deallocate memory
	void cleanup();

};

