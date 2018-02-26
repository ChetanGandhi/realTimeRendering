Pyramid and Cube Texture
========================

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
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include pyramidAndCubeTexture.cpp /link resources\resource.res /LIBPATH:%GLEW_LIB_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib glew32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` to ```9``` key to increase the Texture speed.

###### Preview
- Pyramid and Cube Texture

    ![pyramidAndCubeTexture][pyramidAndCubeTexture-image]

[//]: # "Image declaration"

[pyramidAndCubeTexture-image]: ./preview/pyramidAndCubeTexture.png "Pyramid and Cube Texture"
