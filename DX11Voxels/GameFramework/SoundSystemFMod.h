#pragma once
#include "Exports.h"

namespace FMOD {
	class System;
	class Sound;
}


namespace ZGame {

	class GAMEFRAMEWORK_API SoundSystemFMod
	{
	public:
		FMOD::System* fmodSystem;


		SoundSystemFMod();

		FMOD::Sound* CreateSoundFromFile(const char* filePath);
		void PlaySound(FMOD::Sound* sound, bool loop = false);
		void ReleaseSound(FMOD::Sound* sound);

		void Update();
	};

}
