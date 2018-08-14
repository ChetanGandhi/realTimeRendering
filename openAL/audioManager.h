#pragma once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <al.h>
#include <alc.h>

#include "lib/CWaves.h"

class AudioManager
{
    public:
        AudioManager();
        ~AudioManager();

        ALboolean initialize();
        ALboolean loadWaveAudio(const char* filePath, ALuint bufferId);

        void setListenerPosition(float x, float y, float z);
        void setListenerVelocity(float x, float y, float z);

    private:
        FILE *logFile = NULL;
        CWaves *waveLoader = NULL;

        ALCdevice *device = NULL;
        ALCcontext *context = NULL;

        void log(const char* message, ...);
        void cleanUp();
};
