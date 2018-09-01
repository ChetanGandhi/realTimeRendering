# Triangle and Square by Triangle Strip

In this assignment, we are using `D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP` to draw the square.

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
cl.exe /EHsc /DUNICODE /Zi trianglePerspective.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.

###### Preview

-   Triangle and Square by Triangle Strip

    ![triangleAndSquare][triangleandsquarebytrianglestrip-image]

[//]: # "Image declaration"
[triangleandsquarebytrianglestrip-image]: ./preview/triangleAndSquareByTriangleStrip.png "Triangle and Square by Triangle Strip"
