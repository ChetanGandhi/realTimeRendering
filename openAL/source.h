#pragma once

#include <al.h>
#include <alc.h>

class Source
{
    public:
        Source(ALuint buffer);
        ~Source();

        void setPosition(float x, float y, float z);
        void setVolume(float volume);
        void setPitch(float pitch);
        void play();
        void pause();
        void resume();
        void stop();
        void loop(ALboolean loop);

        ALboolean isPlaying();
        ALboolean isPaused();
        ALboolean isStoped();
        ALboolean isLooping();

    private:
        ALuint sourceId = 0;
        ALboolean loopingEnabled = AL_FALSE;
};
