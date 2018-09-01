# Triangle and Square Rotation

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
cl.exe /EHsc /DUNICODE /Zi triangleAndSquareRotation.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.
-   Press `1` to `9` key to increase the rotation speed.

###### Preview

-   Triangle and Square

    ![triangleAndSquareRotation][triangle-and-square-rotation-image]

[//]: # "Image declaration"
[triangle-and-square-rotation-image]: ./preview/triangleAndSquareRotation.png "Triangle and Square Rotation"
