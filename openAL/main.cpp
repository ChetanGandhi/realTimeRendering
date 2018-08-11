#include <windows.h>
#include <conio.h>

#include "audioManager.h"
#include "source.h"

int main(int argc, char const *argv[])
{
    bool done = false;
    ALfloat x = 0.0f;
    ALfloat y = 0.0f;
    ALfloat z = 0.0f;

    if(argc != 2)
    {
        printf("\n\tUsage: %s waveFilePath.wav\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ALuint audioBufferId = 0;
	Source *source = NULL;

    AudioManager *audioManage = new AudioManager();
    audioManage->init();
    audioManage->setListenerPosition(0.0f, 0.0f, 0.0f);
    audioManage->setListenerVelocity(0.0f, 0.0f, 0.0f);

    alGenBuffers(1, &audioBufferId);

    ALboolean loaded = audioManage->loadWaveAudio(argv[1], audioBufferId);

    if(loaded)
    {
        printf("\nq: quit\n");
        printf("p: pause\n");
        printf("r: resume\n");
        printf("s: start or stop playing\n");
        printf("l: toggle looping\n");
        printf("x: set source x position\n");
        printf("y: set source y position\n");
        printf("z: set source z position\n");

        source = new Source(audioBufferId);
        source->setSourcef(AL_ROLLOFF_FACTOR, 1.0f);
        source->setSourcef(AL_REFERENCE_DISTANCE, 1.0f);
        source->play();

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
                    if(!source->isStoped())
                    {
                        source->stop();
                    }

                    done = true;
                    printf("quit\n");
                break;

                case 'p':
                    source->pause();
                    printf("paused\n");
                break;

                case 'r':
                    source->resume();
                    printf("resuming\n");
                break;

                case 's':
                    if(!source->isStoped())
                    {
                        source->stop();
                        printf("stoped\n");
                    }
                    else
                    {
                        source->play();
                        printf("playing...\n");
                    }
                break;

                case 'l':
                    if(source->isLooping())
                    {
                        source->loop(AL_FALSE);
                        printf("looping disabled\n");
                    }
                    else
                    {
                        source->loop(AL_TRUE);
                        printf("looping enabled\n");
                    }

                break;

                case 'x':
                    printf("\nx position: ");
                    scanf("%f", &x);
                    source->setPosition(x, y, z);

                break;

                case 'y':
                    printf("\ny position: ");
                    scanf("%f", &y);
                    source->setPosition(x, y, z);

                break;

                case 'z':
                    printf("\nz position: ");
                    scanf("%f", &z);
                    source->setPosition(x, y, z);

                break;

                default:
                break;
            }
            fflush(stdin);

	    } while (!done);
    }

    printf("\n");

    delete source;
    source = NULL;

    alDeleteBuffers(1, &audioBufferId);
    audioBufferId = 0;

    delete audioManage;
    audioManage = NULL;

    return 0;
}
