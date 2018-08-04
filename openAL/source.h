#pragma once

#include <al.h>
#include <alc.h>

class Source
{
    public:
        Source();
        ~Source();

        void setPosition(float x, float y, float z);
        void setVolume(float volume);
        void setPitch(float pitch);
        void play(ALuint buffer);
        void pause(ALboolean pause);
        void stop();
        ALboolean isPlaying();

    private:
        ALuint sourceId = 0;
};
