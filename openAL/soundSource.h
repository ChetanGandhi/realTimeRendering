#pragma once

#include <al.h>
#include <alc.h>

class SoundSource
{
    public:
        SoundSource();
        ~SoundSource();

        void setPosition(ALfloat position[3]);
        void setVolume(ALfloat volume);
        void setPitch(ALfloat pitch);
        void setSourcei(ALenum property, ALint value);
        void setSourceiv(ALenum property, ALint *values);
        void setSourcef(ALenum property, ALfloat value);
        void setSourcefv(ALenum property, ALfloat *values);
        void getSourcei(ALenum property, ALint *value);
        void getSourceiv(ALenum property, ALint *values);
        void getSourcef(ALenum property, ALfloat *value);
        void getSourcefv(ALenum property, ALfloat *values);
        void play(ALuint buffer);
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
