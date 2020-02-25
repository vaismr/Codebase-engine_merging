#include "AudioEngine.h"

Implementation::Implementation()
{
	studioSystem = NULL;
	AudioEngine::ErrorCheck(FMOD::Studio::System::create(&studioSystem));
	AudioEngine::ErrorCheck(studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	coreSystem = NULL;
	AudioEngine::ErrorCheck(studioSystem->getCoreSystem(&coreSystem));
}

Implementation::~Implementation()
{
	AudioEngine::ErrorCheck(studioSystem->unloadAll());
	AudioEngine::ErrorCheck(studioSystem->release());
}

void Implementation::Update()
{
	//deletes channels that re no longer playing sounds, free them up
	vector<ChannelMap::iterator> stoppedChannels;

	for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
	{
		bool isPLaying = false;
		it->second->isPlaying(&isPLaying);

		if (!isPLaying)
			stoppedChannels.push_back(it);
	}

	for (auto& it : stoppedChannels)
		channels.erase(it);

	AudioEngine::ErrorCheck(studioSystem->update());
}

Implementation* imp = nullptr;

void AudioEngine::Init()
{
	imp = new Implementation();
}

void AudioEngine::Update()
{
	imp->Update();
}

void AudioEngine::LoadSound(const string& soundName, bool threeD, bool looping, bool stream)
{
	auto foundIt = imp->sounds.find(soundName);

	if (foundIt != imp->sounds.end())
		return;

	FMOD_MODE mode = FMOD_DEFAULT;
	mode |= threeD ? FMOD_3D : FMOD_2D;
	mode |= looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* sound = nullptr;
	AudioEngine::ErrorCheck(imp->coreSystem->createSound(soundName.c_str(), mode, nullptr, &sound));

	if (sound)
		imp->sounds[soundName] = sound;
}

void AudioEngine::UnloadSound(const string& soundName)
{
	auto foundIt = imp->sounds.find(soundName);
	if (foundIt == imp->sounds.end())
		return;

	AudioEngine::ErrorCheck(foundIt->second->release());
	imp->sounds.erase(foundIt);
}

int AudioEngine::PlaySound(const string& soundName, const Vector3& position, float volumedB)
{
	int channelID = imp->nextChannelId++;
	auto foundIt = imp->sounds.find(soundName);

	//if not found in map, load it
	if (foundIt == imp->sounds.end())
	{
		LoadSound(soundName);

		foundIt = imp->sounds.find(soundName);

		//still not found then somethings gone wrong
		if (foundIt == imp->sounds.end())
			return channelID;
	}

	FMOD::Channel* channel = nullptr;
	AudioEngine::ErrorCheck(imp->coreSystem->playSound(foundIt->second, nullptr, true, &channel)); //sets sound in the channel

	if (channel)
	{
		FMOD_MODE crntMode;
		foundIt->second->getMode(&crntMode);

		if (crntMode & FMOD_3D)
		{
			FMOD_VECTOR posFM = VectorToFmod(position);
			AudioEngine::ErrorCheck(channel->set3DAttributes(&posFM, nullptr));
		}

		AudioEngine::ErrorCheck(channel->setVolume(dbToVolume(volumedB)));
		AudioEngine::ErrorCheck(channel->setPaused(false));
		imp->channels[channelID] = channel;
	}

	return channelID;
}

void AudioEngine::SetChannel3DPosition(int channelID, const Vector3& position)
{
	auto foundIt = imp->channels.find(channelID);
	if (foundIt == imp->channels.end())
		return;

	FMOD_VECTOR posFM = VectorToFmod(position);
	AudioEngine::ErrorCheck(foundIt->second->set3DAttributes(&posFM, NULL));
}

void AudioEngine::SetChannelVolume(int channelID, float volumedB)
{
	auto foundIt = imp->channels.find(channelID);
	if (foundIt == imp->channels.end())
		return;

	AudioEngine::ErrorCheck(foundIt->second->setVolume(dbToVolume(volumedB)));
}

void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	auto foundIt = imp->banks.find(bankName);
	if (foundIt == imp->banks.end())
		return;

	FMOD::Studio::Bank* bank;
	AudioEngine::ErrorCheck(imp->studioSystem->loadBankFile(bankName.c_str(), flags, &bank));

	if (bank)
		imp->banks[bankName] = bank;
}

void AudioEngine::LoadEvent(const string& eventName)
{
	//TODO: abstract this find function as its repeaqted often
	
	auto foundIt = imp->events.find(eventName);
	if (foundIt != imp->events.end())
		return;

	FMOD::Studio::EventDescription* eventDescription = NULL;
	AudioEngine::ErrorCheck(imp->studioSystem->getEvent(eventName.c_str(), &eventDescription));

	if (eventDescription)
	{
		FMOD::Studio::EventInstance* eventInstance = NULL;
		AudioEngine::ErrorCheck(eventDescription->createInstance(&eventInstance));

		if (eventInstance)
			imp->events[eventName] = eventInstance;
	}
}

void AudioEngine::PlayEvent(const string& eventName)
{

}

void AudioEngine::StopEvent(const string& eventName, bool immediate)
{

}

bool AudioEngine::IsEventPlaying(const string& eventName)const
{

}

void AudioEngine::GetEventParameter(const string& eventName, const string& parameterName, float* paramater)
{

}

void AudioEngine::SetEventParameter(const string& eventName, const string& parameterName, float value)
{

}

FMOD_VECTOR AudioEngine::VectorToFmod(const Vector3& position)
{
	FMOD_VECTOR FMVec;

	FMVec.x = position.x;
	FMVec.y = position.y;
	FMVec.z = position.z;

	return FMVec;
}

float AudioEngine::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}

float AudioEngine::VolumeTodb(float volume)
{
	return 20.0f * log10f(volume);
}

int AudioEngine::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		cout << "FMOD ERROR" << result << endl; //TODO: remove cout before going gold, keep until then#
		return 1;
	}

	return 0;
}

void AudioEngine::Shutdown()
{
	delete imp;
}