# Rotating Cube with Light

###### How to compile

*   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

*   Now compile the program with resource file and glew32.lib

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include rotatingCubeWithLight.cpp /link resources\resource.res /LIBPATH:%GLEW_LIB_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib glew32.lib
```

###### Keyboard shortcuts

*   Press `Esc` key to quit.
*   Press `f` key to toggle fullscreen mode.
*   Press `1` to `9` key to increase the speed.
*   Press `l` key to enable lighting.
*   Press `a` key to enable animation.

###### Preview

*   Rotating cube with light disabled

    ![rotatingCubeWithLightDisabled][rotatingcubewithlightdisabled-image]

*   Rotating cube with light enabled

    ![rotatingCubeWithLightEnabled][rotatingcubewithlightenabled-image]

[//]: # "Image declaration"
[rotatingcubewithlightdisabled-image]: ./preview/rotatingCubeWithLightDisabled.png "Rotating Cube with Light Disabled"
[rotatingcubewithlightenabled-image]: ./preview/rotatingCubeWithLightEnabled.png "Rotating Cube with Light Enabled"
