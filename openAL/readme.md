# Audio playback using OpenAL

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program.

```
cl.exe /EHsc /DUNICODE /Zi /Fe:main.exe /I %OPENAL_PATH%\include *.cpp lib/*.cpp /link resources\resource.res /LIBPATH:%OPENAL_LIB_PATH% user32.lib kernel32.lib gdi32.lib OpenAL32.lib
```

###### Usage

```
main.exe waveFilePath.wav
```

###### Keyboard shortcuts

-   Press `q` to quit.
-   Press `p` to pause playback.
-   Press `r` to resume playback.
-   Press `s` to start or stop playback.
-   Press `l` to toggle looping.

###### Preview

-   Audio playback

    ![audioPlayback][audio-playback-image]

[//]: # "Image declaration"
[audio-playback-image]: ./preview/audioPlayback.png "Audio playback"
