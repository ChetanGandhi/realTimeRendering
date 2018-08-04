#include <windows.h>
#include <conio.h>

#include "audioManager.h"
#include "source.h"

int main(int argc, char const *argv[])
{
    bool done = false;

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
        printf("space: start or restart playing\n");
        printf("p: pause\n");
        printf("r: resume\n");
        printf("s: stop\n");
        printf("l: toggle looping\n");

        source = new Source(audioBufferId);
        source->play();
        printf("\nplaying...\n");
        printf(":");

        char c = ' ';
        do
    	{
            while(!_kbhit())
            {
                // Do nothing, wait for input.
            }

            c = getche();
            printf("\n:");

            switch(c)
            {
                case 'q':
                    done = true;
                    printf("quit");
                break;

                case ' ':
                    source->play();
                    printf("playing...");
                break;

                case 'p':
                    source->pause();
                    printf("paused");
                break;

                case 'r':
                    source->resume();
                    printf("resuming");
                break;

                case 's':
                    source->stop();
                    printf("stoped");
                break;

                case 'l':
                    if(source->isLooping())
                    {
                        source->loop(AL_FALSE);
                        printf("looping disabled");
                    }
                    else
                    {
                        source->loop(AL_TRUE);
                        printf("looping enabled");
                    }

                break;

                default:
                break;

                fflush(stdin);
            }

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
