# Interleaved Cube using buffersubdata

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file and glew32.lib

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include interleavedBuffersubdata.cpp /link resources\resource.res /LIBPATH:%GLEW_LIB_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib glew32.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.
-   Press `1` to `9` key to increase the speed.
-   Press `a` to toggle animation.
-   Press `l` to toggle lighting.

###### Preview

-   Light disabled

    ![lightDisabled][light-disabled-image]

-   Light enabled

    ![lightEnabled][light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light disabled"
[light-enabled-image]: ./preview/lightEnabled.png "Light enabled"
