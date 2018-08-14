#include <windows.h>
#include <conio.h>

#include "audioManager.h"
#include "soundSource.h"

int main(int argc, char const *argv[])
{
    bool done = false;
    ALfloat soundSourcePosition[3] = {0.0f, 0.0f, 0.0f};
    ALuint audioBufferId = 0;
    ALuint defaultAudioBufferId_1 = 0;
    ALuint defaultAudioBufferId_2 = 0;
    ALuint defaultAudioBufferId_3 = 0;
    ALuint currentAudioBufferId = 0;
    AudioManager *audioManage = NULL;
    SoundSource *soundSource = NULL;

    if(argc != 2)
    {
        printf("\n\tYou can also pass wav file as command line arguments to play it.\n");
        printf("\n\tUsage: %s waveFilePath.wav\n", argv[0]);
    }

    audioManage = new AudioManager();
    audioManage->initialize();
    audioManage->setListenerPosition(0.0f, 0.0f, 0.0f);
    audioManage->setListenerVelocity(0.0f, 0.0f, 0.0f);

    if(argc == 2)
    {
        alGenBuffers(1, &audioBufferId);
        ALboolean userAudioLoaded = audioManage->loadWaveAudio(argv[1], audioBufferId);

        if(userAudioLoaded)
        {
            currentAudioBufferId = audioBufferId;
        }
        else
        {
            printf("\nNot able to open wave file '%s'\n", argv[1]);
            alDeleteBuffers(1, &audioBufferId);
            audioBufferId = 0;
        }
    }

    alGenBuffers(1, &defaultAudioBufferId_1);
    alGenBuffers(1, &defaultAudioBufferId_2);
    alGenBuffers(1, &defaultAudioBufferId_3);

    ALboolean waveDataLoaded = audioManage->loadWaveAudio("resources/audio/sound1.wav", defaultAudioBufferId_1);
    waveDataLoaded = audioManage->loadWaveAudio("resources/audio/sound2.wav", defaultAudioBufferId_2);
    waveDataLoaded = audioManage->loadWaveAudio("resources/audio/sound3.wav", defaultAudioBufferId_3);

    if(waveDataLoaded)
    {
        printf("\nq: quit\n");
        printf("p: play, pause, or resume\n");
        printf("s: stop\n");
        printf("l: toggle looping\n");
        printf("x: set source x position\n");
        printf("y: set source y position\n");
        printf("z: set source z position\n");
        printf("n: switch to next audio\n");

        soundSource = new SoundSource();
        soundSource->setSourcef(AL_ROLLOFF_FACTOR, 1.0f);
        soundSource->setSourcef(AL_REFERENCE_DISTANCE, 1.0f);

        printf(":");

        char c = ':';
        do
    	{
            scanf("%c", &c);
            printf(":");

            switch(c)
            {
                case 'q':
                    if(!soundSource->isStoped())
                    {
                        soundSource->stop();
                    }

                    done = true;
                    printf("quit\n");

                break;

                case 'p':
                    if(soundSource->isPlaying())
                    {
                        soundSource->pause();
                        printf("paused\n");
                    }
                    else if(soundSource->isPaused())
                    {
                        soundSource->resume();
                        printf("resuming\n");
                    }
                    else
                    {
                        soundSource->play(currentAudioBufferId);
                        printf("playing\n");
                    }

                break;

                case 's':
                    if(!soundSource->isStoped())
                    {
                        soundSource->stop();
                        printf("stoped\n");
                    }

                break;

                case 'l':
                    if(soundSource->isLooping())
                    {
                        soundSource->loop(AL_FALSE);
                        printf("looping disabled\n");
                    }
                    else
                    {
                        soundSource->loop(AL_TRUE);
                        printf("looping enabled\n");
                    }

                break;

                case 'x':
                    printf("\nx position: ");
                    scanf("%f", &soundSourcePosition[0]);
                    soundSource->setPosition(soundSourcePosition);

                break;

                case 'y':
                    printf("\ny position: ");
                    scanf("%f", &soundSourcePosition[1]);
                    soundSource->setPosition(soundSourcePosition);

                break;

                case 'z':
                    printf("\nz position: ");
                    scanf("%f", &soundSourcePosition[2]);
                    soundSource->setPosition(soundSourcePosition);

                break;

                case 'n':
                    soundSource->stop();

                    if(currentAudioBufferId == audioBufferId)
                    {
                        currentAudioBufferId = defaultAudioBufferId_1;
                        printf("playing default 1\n");
                    }
                    else if(currentAudioBufferId == defaultAudioBufferId_1)
                    {
                        currentAudioBufferId = defaultAudioBufferId_2;
                        printf("playing default 2\n");
                    }
                    else if(currentAudioBufferId == defaultAudioBufferId_2)
                    {
                        currentAudioBufferId = defaultAudioBufferId_3;
                        printf("playing default 3\n");
                    }
                    else if(currentAudioBufferId == defaultAudioBufferId_3 && audioBufferId != 0)
                    {
                        currentAudioBufferId = audioBufferId;
                        printf("playing user audio\n");
                    }
                    else
                    {
                        currentAudioBufferId = defaultAudioBufferId_1;
                        printf("playing default 1\n");
                    }

                    soundSource->play(currentAudioBufferId);

                break;

                default:
                break;
            }

            fflush(stdin);
	    } while (!done);
    }
    else
    {
        printf("\nUnable to open default sounds.\n");
    }

    printf("\nCleaning up...\n");

    currentAudioBufferId = 0;

    if(soundSource != NULL)
    {
        delete soundSource;
        soundSource = NULL;
    }

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

    if(audioManage != NULL)
    {
        delete audioManage;
        audioManage = NULL;
    }

    return 0;
}
