#include "audioManager.h"

AudioManager::AudioManager()
{
    if (fopen_s(&logFile, "open_al_debug.log", "w") != 0)
	{
		MessageBox(NULL, TEXT("Unable to open log file."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
        cleanUp();
		exit(EXIT_FAILURE);
	}

    log("---------- AudioManager | OpenAL Debug Logs Starts ----------\n");

    waveLoader = new CWaves();

    if(waveLoader == NULL)
    {
        log("[Error] | Not able to create wave loader.");
    }
}

ALboolean AudioManager::initialize()
{
    ALboolean initializationCompleted = AL_FALSE;
    device = alcOpenDevice(NULL);

    if(device)
    {
        log("[Info]: Audio device created: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));
        context = alcCreateContext(device, NULL);

        if(context)
        {
            alcMakeContextCurrent(context);
            initializationCompleted = AL_TRUE;
            log("[Info]: Context created.\n");
        }
        else
        {
            alcCloseDevice(device);
            log("[Error]: Cannot create context.\n");
        }
    }
    else
    {
        log("[Error]: Cannot create audio device.\n");
    }

    return initializationCompleted;
}

ALboolean AudioManager::loadWaveAudio(const char* filePath, ALuint bufferId)
{
    ALboolean loaded = AL_FALSE;
	WAVEID waveId = 0;
	ALint dataSize = 0;
    ALint frequency = 0;
	ALenum bufferFormat;
	ALchar *data = NULL;

    if(waveLoader)
    {
        WAVERESULT loadWaveResult = waveLoader->LoadWaveFile(filePath, &waveId);

        if(SUCCEEDED(loadWaveResult))
        {
            WAVERESULT waveSizeResult = waveLoader->GetWaveSize(waveId, (unsigned long *)&dataSize);
            WAVERESULT waveDataResult = waveLoader->GetWaveData(waveId, (void **)&data);
            WAVERESULT waveFrequencyResult = waveLoader->GetWaveFrequency(waveId, (unsigned long *)&frequency);
            WAVERESULT waveBufferFormatResult = waveLoader->GetWaveALBufferFormat(waveId, &alGetEnumValue, (unsigned long *)&bufferFormat);

            log("[Info] | Buffer format: %d\n", bufferFormat);

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
                    log("[Info] | wav file loaded: %s", filePath);
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

            waveLoader->DeleteWaveFile(waveId);
        }
        else
        {
            log("[Error]: Not able to load audio file '%s'\n", filePath);
        }
    }

    return loaded;
}

void AudioManager::setListenerPosition(float x, float y, float z)
{
    alListener3f(AL_POSITION, x, y, z);
}

void AudioManager::setListenerVelocity(float x, float y, float z)
{
    alListener3f(AL_VELOCITY, x, y, z);
}

void AudioManager::log(const char* message, ...)
{
    va_list args;
    va_start(args, message );
    vfprintf(logFile, message, args );
    va_end( args );

    fflush(logFile);
}

void AudioManager::cleanUp()
{
    ALCcontext *pContext = alcGetCurrentContext();
	ALCdevice *pDevice = alcGetContextsDevice(pContext);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);

    if (waveLoader)
	{
		delete waveLoader;
		waveLoader = NULL;
        log("[Info]: Audio device destroyed.\n");
	}

    if(logFile)
    {
        log("---------- AudioManager | OpenAL Debug Logs End ----------\n");
        fclose(logFile);
        logFile = NULL;
    }
}

AudioManager::~AudioManager()
{
    cleanUp();
}
