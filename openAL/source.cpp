#include "source.h"

Source::Source(ALuint buffer)
{
    alGenSources(1, &sourceId);
    alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcef(sourceId, AL_GAIN, 1.0f);
    alSourcef(sourceId, AL_PITCH, 1.0f);
    alSourcei(sourceId, AL_BUFFER, buffer);
}

void Source::setPosition(ALfloat x, ALfloat y, ALfloat z)
{
    alSource3f(sourceId, AL_POSITION, x, y, z);
}

void Source::setVolume(ALfloat volume)
{
    alSourcef(sourceId, AL_GAIN, volume);
}

void Source::setPitch(ALfloat pitch)
{
    alSourcef(sourceId, AL_PITCH, pitch);
}

void Source::setSourcef(ALenum property, ALfloat value)
{
    alSourcef(sourceId, property, value);
}

void Source::play()
{
    stop();
    alSourcePlay(sourceId);
}

void Source::pause()
{
    if(isPlaying())
    {
        alSourcePause(sourceId);
    }
}

void Source::resume()
{
    if(isPaused())
    {
        alSourcePlay(sourceId);
    }
}

void Source::stop()
{
    if(!isStoped())
    {
        alSourceStop(sourceId);
    }
}

void Source::loop(ALboolean loop)
{
    loopingEnabled = loop;
    alSourcei(sourceId, AL_LOOPING, loop);
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

ALboolean Source::isPaused()
{
    ALint paused = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &paused);

    if(paused == AL_PAUSED)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean Source::isStoped()
{
    ALint stoped = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &stoped);

    if(stoped == AL_STOPPED)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean Source::isLooping()
{
    return loopingEnabled;
}

Source::~Source()
{
    stop();
    alSourcei(sourceId, AL_BUFFER, 0);
    alDeleteSources(1, &sourceId);
}
