#include <windows.h>

#include "audioManager.h"
#include "source.h"

int main(int argc, char const *argv[])
{
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

    ALboolean loaded = audioManage->loadWaveFile(argv[1], audioBufferId);

    if(loaded)
    {
        source = new Source();
        source->play(audioBufferId);
        printf("playing");

        do
    	{
	    	Sleep(1000);
		    printf(".");
	    } while (source->isPlaying());
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
