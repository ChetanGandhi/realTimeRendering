# Pyramid with Multiple Light

###### How to compile

*   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

*   Now compile the program.

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include pyramidMultipleLight.cpp /link resources\resource.res /LIBPATH:%GLEW32_LIB_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glew32.lib
```

###### Keyboard shortcuts

*   Press `Esc` key to quit.
*   Press `1` to `9` key to increase the Texture speed.
*   Press `f` key to toggle fullscreen mode.
*   Press `a` key to toggle animation.
*   Press `l` key to toggle lighting.

###### Preview

*   Pyramid Light Disabled

    ![pyramidLightDisabled][pyramid-light-disabled-image]

*   Pyramid Light Enabled

    ![pyramidLightEnabled][pyramid-light-enabled-image]

[//]: # "Image declaration"
[pyramid-light-disabled-image]: ./preview/pyramidLightDisabled.png "Pyramid Light Disabled"
[pyramid-light-enabled-image]: ./preview/pyramidLightEnabled.png "Pyramid Light Enabled"
