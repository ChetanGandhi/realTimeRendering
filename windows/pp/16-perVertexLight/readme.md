# Per Vertex Light

###### How to compile

*   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

*   Now compile the program with resource file and glew32.lib
*   As we are using x86 version of `Sphere.lib` and `Sphere.dll`, hence we have to use x86 version of all other dll's and lib's.

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include /I %SPHERE_PATH% perVertexLight.cpp /link resources\resource.res /LIBPATH:%GLEW32_LIB_PATH% /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glew32.lib Sphere.lib
```

###### Keyboard shortcuts

*   Press `Esc` key to quit.
*   Press `f` key to toggle fullscreen mode.
*   Press `l` key to enable lighting.

###### Preview

*   Per Vertex Light Disabled

    ![perVertexLightDisabled][pervertexlightdisabled-image]

*   Per Vertex Light Enabled

    ![perVertexLightEnabled][pervertexlightenabled-image]

[//]: # "Image declaration"
[pervertexlightdisabled-image]: ./preview/perVertexLightDisabled.png "Per Vertex Light Disabled"
[pervertexlightenabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
