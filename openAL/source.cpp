#include "source.h"

Source::Source()
{
    alGenSources(1, &sourceId);
    alSource3f(sourceId, AL_POSITION, 0, 0, 0);
    alSourcef(sourceId, AL_GAIN, 1.0f);
    alSourcef(sourceId, AL_PITCH, 1.0f);
}

void Source::setPosition(float x, float y, float z)
{
    alSource3f(sourceId, AL_POSITION, x, y, z);
}

void Source::setVolume(float volume)
{
    alSourcef(sourceId, AL_GAIN, volume);
}

void Source::setPitch(float pitch)
{
    alSourcef(sourceId, AL_PITCH, pitch);
}

void Source::play(ALuint buffer)
{
    stop();
    alSourcei(sourceId, AL_BUFFER, buffer);
    alSourcePlay(sourceId);
}

void Source::pause(ALboolean pause)
{
    if(isPlaying())
    {
        alSourcePause(sourceId);
    }
    else
    {
        alSourcePlay(sourceId);
    }
}

void Source::stop()
{
    if(isPlaying())
    {
        alSourceStop(sourceId);
    }
}

ALboolean Source::isPlaying()
{
    ALint playing = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &playing);

    if(playing == AL_PLAYING)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

Source::~Source()
{
    stop();
    alDeleteSources(1, &sourceId);
}
