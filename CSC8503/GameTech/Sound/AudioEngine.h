#pragma once

//checks if it has been defined before, prevents multiple definitions of the objects in our header
#ifndef _AUDIO_ENGINE_H_

#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

struct Vec3
{
	float x;
	float y;
	float z;
};

struct Implementation
{
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* studioSystem;
	FMOD::System* coreSystem;

	int nextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;
	typedef map<string, FMOD::Studio::EventInstance*> EventMap;
	typedef map<string, FMOD::Studio::Bank*> BankMap;

	BankMap banks;
	EventMap events;
	SoundMap sounds;
	ChannelMap channels;
};

class AudioEngine
{
public:
	static void Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);

	void LoadBank(const string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);

	void LoadEvent(const string& eventName);
	void PlayEvent(const string& eventName);
	void StopEvent(const string& eventName, bool immediate = false);
	void GetEventParameter(const string& eventName, const string& eventParameter, float* parameter);
	void SetEventParameter(const string& eventName, const string& parameterName, float value);

	void LoadSound(const string& soundName, bool threeD = true, bool looping = false, bool stream = false);
	void UnloadSound(const string& soundName);
	int PlaySounds(const string& soundName, const Vec3& pos = Vec3{ 0,0,0 }, float volumedB = 0.0f);

	void Set3DListenerAndOrientation(const Vec3& pos, const Vec3& vel, const Vec3& forward, const Vec3& up, float volumedB = 0.0f);

	bool IsPaused(int channelID);
	void PlayChannel(int channelID, bool pause = false);
	void StopChannel(int channelID);
	void StopAllChannels();
	void TogglePauseAllChannels();
	void SetChannel3DPosition(int channelID, const Vec3& position);
	void SetChannelVolume(int channelID, float volumedB);

	bool IsPlaying(int channelID) const;
	bool IsEventPlaying(const string& eventName) const;

	float dbToVolume(float db);
	float VolumeTodb(float volume);

	FMOD_VECTOR VectorToFmod(const Vec3& position);

	void PrintListenerPos(int ID = 0);

private:
	bool paused = false;
};

#endif

