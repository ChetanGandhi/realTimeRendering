#include "soundSource.h"

SoundSource::SoundSource()
{
    alGenSources(1, &sourceId);
    alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcef(sourceId, AL_GAIN, 1.0f);
    alSourcef(sourceId, AL_PITCH, 1.0f);
}

void SoundSource::setPosition(ALfloat position[3])
{
    alSourcefv(sourceId, AL_POSITION, position);
}

void SoundSource::setVolume(ALfloat volume)
{
    alSourcef(sourceId, AL_GAIN, volume);
}

void SoundSource::setPitch(ALfloat pitch)
{
    alSourcef(sourceId, AL_PITCH, pitch);
}

void SoundSource::setSourcei(ALenum property, ALint value)
{
    alSourcei(sourceId, property, value);
}

void SoundSource::setSourceiv(ALenum property, ALint *values)
{
    alSourceiv(sourceId, property, values);
}

void SoundSource::setSourcef(ALenum property, ALfloat value)
{
    alSourcef(sourceId, property, value);
}

void SoundSource::setSourcefv(ALenum property, ALfloat* values)
{
    alSourcefv(sourceId, property, values);
}

void SoundSource::getSourcei(ALenum property, ALint *value)
{
    alGetSourcei(sourceId, property, value);
}

void SoundSource::getSourceiv(ALenum property, ALint *values)
{
    alGetSourceiv(sourceId, property, values);
}

void SoundSource::getSourcef(ALenum property, ALfloat *value)
{
    alGetSourcef(sourceId, property, value);
}

void SoundSource::getSourcefv(ALenum property, ALfloat *values)
{
    alGetSourcefv(sourceId, property, values);
}

void SoundSource::play(ALuint buffer)
{
    stop();
    alSourcei(sourceId, AL_BUFFER, buffer);
    alSourcePlay(sourceId);
}

void SoundSource::pause()
{
    if(isPlaying())
    {
        alSourcePause(sourceId);
    }
}

void SoundSource::resume()
{
    if(isPaused())
    {
        alSourcePlay(sourceId);
    }
}

void SoundSource::stop()
{
    if(!isStoped())
    {
        alSourceStop(sourceId);
    }
}

void SoundSource::loop(ALboolean loop)
{
    loopingEnabled = loop;
    alSourcei(sourceId, AL_LOOPING, loop);
}

ALboolean SoundSource::isPlaying()
{
    ALint playing = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &playing);

    if(playing == AL_PLAYING)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean SoundSource::isPaused()
{
    ALint paused = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &paused);

    if(paused == AL_PAUSED)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean SoundSource::isStoped()
{
    ALint stoped = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &stoped);

    if(stoped == AL_STOPPED)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean SoundSource::isLooping()
{
    return loopingEnabled;
}

SoundSource::~SoundSource()
{
    stop();
    alSourcei(sourceId, AL_BUFFER, 0);
    alDeleteSources(1, &sourceId);
}
