# Rotating Cube with Light

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file

```
cl.exe /EHsc /DUNICODE /Zi rotatingCubeWithLight.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.
-   Press `1` to `9` key to increase the speed.
-   Press `l` key to enable lighting.
-   Press `a` key to enable animation.

###### Preview

-   Rotating cube with light disabled

    ![rotatingCubeWithLightDisabled][rotating-cube-with-light-disabled-image]

-   Rotating cube with light enabled

    ![rotatingCubeWithLightEnabled][rotating-cube-with-light-enabled-image]

[//]: # "Image declaration"
[rotating-cube-with-light-disabled-image]: ./preview/rotatingCubeWithLightDisabled.png "Rotating Cube with Light Disabled"
[rotating-cube-with-light-enabled-image]: ./preview/rotatingCubeWithLightEnabled.png "Rotating Cube with Light Enabled"
