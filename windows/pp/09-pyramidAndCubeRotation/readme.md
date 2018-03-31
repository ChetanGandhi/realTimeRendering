Pyramid and Cube Rotation
=========================

###### How to compile

- First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

- Now compile the program with resource file and glew32.lib

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include pyramidAndCubeRotation.cpp /link resources\resource.res /LIBPATH:%GLEW_LIB_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib glew32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview
- Pyramid and Cube Rotation

    ![pyramidAndCubeRotation][pyramidAndCubeRotation-image]

[//]: # "Image declaration"

[pyramidAndCubeRotation-image]: ./preview/pyramidAndCubeRotation.png "Pyramid and Cube Rotation"
