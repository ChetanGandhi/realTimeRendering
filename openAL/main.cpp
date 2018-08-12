#include <windows.h>
#include <conio.h>

#include "audioManager.h"
#include "soundSource.h"

int main(int argc, char const *argv[])
{
    bool done = false;
    ALfloat soundSourcePosition[3] = {0.0f, 0.0f, 0.0f};
    ALuint audioBufferId = 0;
	AudioManager *audioManage = NULL;
    SoundSource *soundSource = NULL;

    if(argc != 2)
    {
        printf("\n\tUsage: %s waveFilePath.wav\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    audioManage = new AudioManager();
    audioManage->init();
    audioManage->setListenerPosition(0.0f, 0.0f, 0.0f);
    audioManage->setListenerVelocity(0.0f, 0.0f, 0.0f);

    alGenBuffers(1, &audioBufferId);

    ALboolean waveDataLoaded = audioManage->loadWaveAudio(argv[1], audioBufferId);

    if(waveDataLoaded)
    {
        printf("\nq: quit\n");
        printf("p: pause\n");
        printf("r: resume\n");
        printf("s: start or stop playing\n");
        printf("l: toggle looping\n");
        printf("x: set source x position\n");
        printf("y: set source y position\n");
        printf("z: set source z position\n");

        soundSource = new SoundSource();
        soundSource->setSourcef(AL_ROLLOFF_FACTOR, 1.0f);
        soundSource->setSourcef(AL_REFERENCE_DISTANCE, 1.0f);
        soundSource->play(audioBufferId);

        printf("\nplaying...\n");
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
                    soundSource->pause();
                    printf("paused\n");
                break;

                case 'r':
                    soundSource->resume();
                    printf("resuming\n");
                break;

                case 's':
                    if(!soundSource->isStoped())
                    {
                        soundSource->stop();
                        printf("stoped\n");
                    }
                    else
                    {
                        soundSource->play(audioBufferId);
                        printf("playing...\n");
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

                default:
                break;
            }

            fflush(stdin);
	    } while (!done);
    }

    printf("\n");

    delete soundSource;
    soundSource = NULL;

    alDeleteBuffers(1, &audioBufferId);
    audioBufferId = 0;

    delete audioManage;
    audioManage = NULL;

    return 0;
}
