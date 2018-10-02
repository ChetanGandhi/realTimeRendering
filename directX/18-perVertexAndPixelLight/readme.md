# Per Vertex and Pixel Light

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file
-   The `Sphere.lib` and `Sphere.dll` is provided by Sir and is not included with this source. You can draw any other shape.
-   As we are using x86 version of `Sphere.lib` and `Sphere.dll`, we have to use x86 version of all other dll's and lib's.

```
cl.exe /EHsc /DUNICODE /Zi /I %SPHERE_PATH% perVertexAndPixelLight.cpp /link resources\resource.res /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib Sphere.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `F11` key to toggle fullscreen mode.
-   Press `l` key to enable lighting.
-   Press `v` key to enable per vertex lighting.
-   Press `f` key to enable per pixel lighting.

###### Preview

-   Light Disabled

    ![lightDisabled][light-disabled-image]

-   Per Vertex Lighting Enabled

    ![perVertexLightEnabled][per-vertex-light-enabled-image]

-   Per Pixel Lighting Enabled

    ![perPixelLightEnabled][per-pixel-light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light Disabled"
[per-vertex-light-enabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
[per-pixel-light-enabled-image]: ./preview/perPixelLightEnabled.png "Per Pixel Light Enabled"
