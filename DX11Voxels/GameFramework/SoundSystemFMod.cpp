#include "SoundSystemFMod.h"

#include <iostream>

#include "fmod.hpp"
#include "fmod_errors.h"


void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line)
{
    if (result != FMOD_OK)
    {
        std::cout << ("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
    }
}

#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)



ZGame::SoundSystemFMod::SoundSystemFMod()
{
    if (FMOD::System_Create(&fmodSystem) != FMOD_OK)
    {
        // Report Error
        return;
    }

    int driverCount = 0;
    fmodSystem->getNumDrivers(&driverCount);

    if (driverCount == 0)
    {
        // Report Error
        return;
    }

    // Initialize our Instance with 36 Channels
    fmodSystem->init(36, FMOD_INIT_NORMAL, nullptr);
}


FMOD::Sound* ZGame::SoundSystemFMod::CreateSoundFromFile(const char* filePath)
{
    FMOD::Sound* ret = nullptr;
    fmodSystem->createSound(filePath, FMOD_DEFAULT, nullptr, &ret);
    return ret;
}


void ZGame::SoundSystemFMod::PlaySound(FMOD::Sound* sound, bool loop)
{
    if (!loop)
        sound->setMode(FMOD_LOOP_OFF);
    else
    {
        sound->setMode(FMOD_LOOP_NORMAL);
        sound->setLoopCount(-1);
    }

    fmodSystem->playSound(sound, nullptr, false, nullptr);
}


void ZGame::SoundSystemFMod::ReleaseSound(FMOD::Sound* sound)
{
    sound->release();
}


void ZGame::SoundSystemFMod::Update()
{
    auto res = fmodSystem->update();
    ERRCHECK(res);
}
