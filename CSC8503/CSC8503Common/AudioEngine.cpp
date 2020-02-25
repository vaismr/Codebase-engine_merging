#include "AudioEngine.h"

Implementation::Implementation()
{
	studioSystem = NULL;
	AudioEngine::ErrorCheck(FMOD::Studio::System::create(&studioSystem));
	AudioEngine::ErrorCheck(studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);

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
