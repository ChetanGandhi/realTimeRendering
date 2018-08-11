#pragma once

#include <al.h>
#include <alc.h>

class Source
{
    public:
        Source(ALuint buffer);
        ~Source();

        void setPosition(ALfloat x, ALfloat y, ALfloat z);
        void setVolume(ALfloat volume);
        void setPitch(ALfloat pitch);
        void setSourcef(ALenum property, ALfloat value);
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
