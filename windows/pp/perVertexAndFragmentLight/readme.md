# Per Vertex and Fragment Light

###### How to compile

*   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

*   Now compile the program with resource file and glew32.lib
*   As we are using x86 version of `Sphere.lib` and `Sphere.dll`, we have to use x86 version of all other dll's and lib's.

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include /I %SPHERE_PATH% perVertexAndFragmentLight.cpp /link resources\resource.res /LIBPATH:%GLEW32_LIB_PATH% /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glew32.lib Sphere.lib
```

###### Keyboard shortcuts

*   Press `Esc` key to quit.
*   Press `F11` key to toggle fullscreen mode.
*   Press `l` key to enable lighting.
*   Press `v` key to enable per vertex lighting.
*   Press `f` key to enable per fragment lighting.

###### Preview

*   Light Disabled

    ![lightDisabled][lightdisabled-image]

*   Per Vertex Lighting Enabled

    ![perVertexLightEnabled][pervertexlightenabled-image]

*   Per Fragment Lighting Enabled

    ![perFragmentLightEnabled][perfragmentlightenabled-image]

[//]: # "Image declaration"
[lightdisabled-image]: ./preview/lightDisabled.png "Light Disabled"
[pervertexlightenabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
[perfragmentlightenabled-image]: ./preview/perFragmentLightEnabled.png "Per Fragment Light Enabled"
