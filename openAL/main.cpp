#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <al.h>
#include <alc.h>

#include "libs/cWaves/CWaves.h"

FILE *logFile = NULL;
CWaves *waveLoader = NULL;
ALCdevice *device = NULL;
ALCcontext *context = NULL;
ALfloat soundSourcePosition[3] = {0.0f, 0.0f, 0.0f};
ALuint audioBufferId = 0;
ALuint defaultAudioBufferId_1 = 0;
ALuint defaultAudioBufferId_2 = 0;
ALuint defaultAudioBufferId_3 = 0;
ALuint currentAudioBufferId = 0;
ALuint soundSourceId = 0;

ALboolean initialize();
ALboolean loadWaveAudio(const char* filePath, ALuint bufferId);
ALboolean isPlaying(ALuint sourceId);
ALboolean isPaused(ALuint sourceId);
ALboolean isStoped(ALuint sourceId);
ALboolean isLooping(ALuint sourceId);

void play(ALuint sourceId, ALuint buffer);
void pause(ALuint sourceId);
void resume(ALuint sourceId);
void stop(ALuint sourceId);
void loop(ALuint sourceId, ALboolean loop);
void cleanUp();
void log(const char* message, ...);

int main(int argc, char const *argv[])
{
    bool done = false;

    if (fopen_s(&logFile, "audio_manager_debug.log", "w") != 0)
	{
		printf("Unable to open audio manager log file.");
	}

    fprintf(logFile, "---------- AudioManager | Debug Logs Starts ----------\n");

    waveLoader = new CWaves();

    if(waveLoader == NULL)
    {
        log("[Error] | Not able to create wave loader.");
    }

    // Initialize the audio manager, this will initialize the audio device and context.
    ALboolean audioManagerInitialized = initialize();

    if(!audioManagerInitialized)
    {
        printf("Not able to create audio device and context");
        cleanUp();
        exit(EXIT_FAILURE);
    }

    // Set listener's position and velocity.
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    // Generate buffers to hold audio data.
    alGenBuffers(1, &defaultAudioBufferId_1);
    alGenBuffers(1, &defaultAudioBufferId_2);
    alGenBuffers(1, &defaultAudioBufferId_3);

    // Check for any errors
    ALenum alError = alGetError();

    if(alError != AL_NO_ERROR)
    {
        printf("Not able to create audio buffers, error: %s\n", alGetString(alError));
        cleanUp();
        exit(EXIT_FAILURE);
    }

    // Reset the error variable.
    alError = AL_NO_ERROR;

    // Load default audio's.
    ALboolean waveDataLoaded = loadWaveAudio("resources/audio/sound1.wav", defaultAudioBufferId_1);
    waveDataLoaded = loadWaveAudio("resources/audio/sound2.wav", defaultAudioBufferId_2);
    waveDataLoaded = loadWaveAudio("resources/audio/sound3.wav", defaultAudioBufferId_3);

    // If user have provided an wav file to play, load it also.
    if(argc != 2)
    {
        printf("\n\tYou can also pass wav file as command line arguments to play it.\n");
        printf("\n\tUsage: %s waveFilePath.wav\n", argv[0]);
        currentAudioBufferId = defaultAudioBufferId_1;
    }
    else
    {
        alGenBuffers(1, &audioBufferId);

        if(alError != AL_NO_ERROR)
        {
            printf("Not able to create buffer for use audio, error: %s", alGetString(alError));
            log("[Error] | Not able to create buffer for use audio, error: %s", alGetString(alError));
        }

        // Reset the error variable.
        alError = AL_NO_ERROR;

        ALboolean userAudioLoaded = loadWaveAudio(argv[1], audioBufferId);

        if(userAudioLoaded)
        {
            currentAudioBufferId = audioBufferId;
        }
        else
        {
            printf("\nNot able to open wave file '%s'\n", argv[1]);
            log("[Error] | Not able to open wave file '%s'\n", argv[1]);
            alDeleteBuffers(1, &audioBufferId);
            audioBufferId = 0;
        }
    }

    if(!waveDataLoaded)
    {
        printf("\nUnable to open default sounds.\n");
    }

    // Create audio source.
    alGenSources(1, &soundSourceId);

    // Check for any errors.
    alError = alGetError();

    if(alError != AL_NO_ERROR)
    {
        printf("Not able to create audio source, error: %s", alGetString(alError));
        cleanUp();
        exit(EXIT_FAILURE);
    }

    // Reset the error variable.
    alError = AL_NO_ERROR;

    // Set audio source rolloff factor and reference distance so that
    // OpenAL can determine how to fade-in and fade-out sound.
    alSourcef(soundSourceId, AL_ROLLOFF_FACTOR, 1.0f);
    alSourcef(soundSourceId, AL_REFERENCE_DISTANCE, 1.0f);

    if(soundSourceId != 0)
    {
        printf("\nq: quit\n");
        printf("p: play, pause, or resume\n");
        printf("s: stop\n");
        printf("l: toggle looping\n");
        printf("x: set source x position\n");
        printf("y: set source y position\n");
        printf("z: set source z position\n");
        printf("n: switch to next audio\n");

        printf(":");

        char c = ':';

        while(!done)
    	{
            scanf("%c", &c);
            printf(":");

            switch(c)
            {
                case 'q':
                    if(!isStoped(soundSourceId))
                    {
                        stop(soundSourceId);
                    }

                    done = true;
                    printf("quit\n");

                break;

                case 'p':
                    if(isPlaying(soundSourceId))
                    {
                        pause(soundSourceId);
                        printf("paused\n");
                    }
                    else if(isPaused(soundSourceId))
                    {
                        resume(soundSourceId);
                        printf("resuming\n");
                    }
                    else if(currentAudioBufferId != 0)
                    {
                        play(soundSourceId, currentAudioBufferId);
                        printf("playing\n");
                    }
                    else
                    {
                        printf("nothing to play\n");
                    }

                break;

                case 's':
                    if(!isStoped(soundSourceId))
                    {
                        stop(soundSourceId);
                        printf("stoped\n");
                    }

                break;

                case 'l':
                    if(isLooping(soundSourceId))
                    {
                        loop(soundSourceId, AL_FALSE);
                        printf("looping disabled\n");
                    }
                    else
                    {
                        loop(soundSourceId, AL_TRUE);
                        printf("looping enabled\n");
                    }

                break;

                case 'x':
                    printf("\nx position: ");
                    scanf("%f", &soundSourcePosition[0]);
                    alSourcefv(soundSourceId, AL_POSITION,  soundSourcePosition);

                break;

                case 'y':
                    printf("\ny position: ");
                    scanf("%f", &soundSourcePosition[1]);
                    alSourcefv(soundSourceId, AL_POSITION,  soundSourcePosition);

                break;

                case 'z':
                    printf("\nz position: ");
                    scanf("%f", &soundSourcePosition[2]);
                    alSourcefv(soundSourceId, AL_POSITION,  soundSourcePosition);

                break;

                case 'n':
                    stop(soundSourceId);

                    if(currentAudioBufferId == audioBufferId)
                    {
                        currentAudioBufferId = defaultAudioBufferId_1;
                        printf("switch to default 1 audio, enter 'p' to start playing\n");
                    }
                    else if(currentAudioBufferId == defaultAudioBufferId_1)
                    {
                        currentAudioBufferId = defaultAudioBufferId_2;
                        printf("switch to default 2 audio, enter 'p' to start playing\n");
                    }
                    else if(currentAudioBufferId == defaultAudioBufferId_2)
                    {
                        currentAudioBufferId = defaultAudioBufferId_3;
                        printf("switch to default 3 audio, enter 'p' to start playing\n");
                    }
                    else if(currentAudioBufferId == defaultAudioBufferId_3 && audioBufferId != 0)
                    {
                        currentAudioBufferId = audioBufferId;
                        printf("switch to user audio audio, enter 'p' to start playing\n");
                    }
                    else
                    {
                        currentAudioBufferId = defaultAudioBufferId_1;
                        printf("switch to default 1 audio, enter 'p' to start playing\n");
                    }

                break;

                default:
                break;
            }

            fflush(stdin);
	    };
    }

    cleanUp();
    return 0;
}

ALboolean initialize()
{
    ALboolean initializationCompleted = AL_FALSE;

    // Open system default audio device
    device = alcOpenDevice(NULL);

    // If device is opened, then create context
    if(device)
    {
        log("[Info] | Audio device created: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));
        context = alcCreateContext(device, NULL);

        // If context is created, then make it as current.
        if(context)
        {
            alcMakeContextCurrent(context);
            initializationCompleted = AL_TRUE;
            log("[Info] | Context created.\n");
        }
        else
        {
            log("[Error]: Cannot create context.\n");
        }
    }
    else
    {
        log("[Error]: Cannot create audio device.\n");
    }

    return initializationCompleted;
}

ALboolean loadWaveAudio(const char* filePath, ALuint bufferId)
{
    ALboolean loaded = AL_FALSE;
	WAVEID waveId = 0;
	ALint dataSize = 0;
    ALint frequency = 0;
	ALenum bufferFormat;
	ALchar *data = NULL;

    if(waveLoader)
    {
        // Load the wave file.
        WAVERESULT loadWaveResult = waveLoader->LoadWaveFile(filePath, &waveId);

        // If wave file is loaded, then get dataSize, frequency, bufferFormat and data from it.
        if(SUCCEEDED(loadWaveResult))
        {
            WAVERESULT waveSizeResult = waveLoader->GetWaveSize(waveId, (unsigned long *)&dataSize);
            WAVERESULT waveDataResult = waveLoader->GetWaveData(waveId, (void **)&data);
            WAVERESULT waveFrequencyResult = waveLoader->GetWaveFrequency(waveId, (unsigned long *)&frequency);
            WAVERESULT waveBufferFormatResult = waveLoader->GetWaveALBufferFormat(waveId, &alGetEnumValue, (unsigned long *)&bufferFormat);

            // If we have all the data, then fill the buffer.
            if(SUCCEEDED(waveSizeResult) && SUCCEEDED(waveDataResult) && SUCCEEDED(waveFrequencyResult) && SUCCEEDED(waveBufferFormatResult))
            {
                // Clear any errors
                alGetError();

                // Load the data to buffer
                alBufferData(bufferId, bufferFormat, data, dataSize, frequency);

                ALenum error =  alGetError();

                if(error == AL_NO_ERROR)
                {
                    loaded = AL_TRUE;
                    log("[Info] | wav file loaded: %s\n", filePath);
                }
                else
                {
                    log("[Error] | Not able to load audio data, error: %d, file: %s", error, filePath);
                }
            }
            else
            {
                log("[Error]: Not able to load wav data\n");
            }

            // Release the wave file data as its been copied to buffer.
            waveLoader->DeleteWaveFile(waveId);
        }
        else
        {
            log("[Error]: Not able to load audio file '%s'\n", filePath);
        }
    }

    return loaded;
}

void play(ALuint sourceId, ALuint buffer)
{
    // Stop current playback.
    stop(sourceId);

    // Build to new buffer which we want to play.
    alSourcei(sourceId, AL_BUFFER, buffer);

    // Start playing.
    alSourcePlay(sourceId);
}

void pause(ALuint sourceId)
{
    // Check is given source playing any audio.
    if(isPlaying(sourceId))
    {
        // Pause current audio playback.
        alSourcePause(sourceId);
    }
}

void resume(ALuint sourceId)
{
    // Check is given source in paused state.
    if(isPaused(sourceId))
    {
        // Resume playing current audio.
        alSourcePlay(sourceId);
    }
}

void stop(ALuint sourceId)
{
    // Check is given source in stoped state.
    if(!isStoped(sourceId))
    {
        // If not then stop current playback.
        alSourceStop(sourceId);
    }
}

void loop(ALuint sourceId, ALboolean loop)
{
    // Set looping state of given source.
    alSourcei(sourceId, AL_LOOPING, loop);
}

ALboolean isPlaying(ALuint sourceId)
{
    // Check is given source state is AL_PLAYING.
    ALint playing = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &playing);

    if(playing == AL_PLAYING)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean isPaused(ALuint sourceId)
{
    // Check is given source state is AL_PAUSED.
    ALint paused = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &paused);

    if(paused == AL_PAUSED)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean isStoped(ALuint sourceId)
{
    // Check is given source state is AL_STOPPED.
    ALint stoped = 0;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &stoped);

    if(stoped == AL_STOPPED)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}

ALboolean isLooping(ALuint sourceId)
{
    // Get looping state of given source.
    ALint looping = AL_FALSE;
    alGetSourcei(sourceId, AL_LOOPING, &looping);

    return looping;
}

void log(const char* message, ...)
{
    if(logFile == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, message );
    fprintf(logFile, "AudioManager | ");
    vfprintf(logFile, message, args );
    va_end( args );

    fflush(logFile);
}

void cleanUp()
{
    printf("\nCleaning up...\n");

    currentAudioBufferId = 0;

    // First clear the sound source as the buffers are attached to it.
    if(soundSourceId != 0)
    {
        alSourcei(soundSourceId, AL_BUFFER, 0);
        alDeleteSources(1, &soundSourceId);
        soundSourceId = 0;
    }

    // Now delete the buffers.
    if(audioBufferId != 0)
    {
        alDeleteBuffers(1, &audioBufferId);
        audioBufferId = 0;
    }

    if(defaultAudioBufferId_1 != 0)
    {
        alDeleteBuffers(1, &defaultAudioBufferId_1);
        defaultAudioBufferId_1 = 0;
    }

    if(defaultAudioBufferId_2 != 0)
    {
        alDeleteBuffers(1, &defaultAudioBufferId_2);
        defaultAudioBufferId_2 = 0;
    }

    if(defaultAudioBufferId_3 != 0)
    {
        alDeleteBuffers(1, &defaultAudioBufferId_3);
        defaultAudioBufferId_3 = 0;
    }

    // Get the current context and check is it ours.
    ALCcontext *currentContext = alcGetCurrentContext();

    // If it is, then clear current context
	if(currentContext == context)
    {
        alcMakeContextCurrent(NULL);
    }

    // Delete the context and audio device.
	alcDestroyContext(context);
	alcCloseDevice(device);

    currentContext = NULL;

    if (waveLoader)
	{
		delete waveLoader;
		waveLoader = NULL;
	}

    if(logFile)
    {
        fprintf(logFile, "---------- AudioManager | Debug Logs End ----------\n");
        fclose(logFile);
        logFile = NULL;
    }
}
