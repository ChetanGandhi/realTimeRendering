# Triangle and Square by Triangle List

In this assignment, we are using `D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST` to draw the square.

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
cl.exe /EHsc /DUNICODE /Zi triangleAndSquareByTriangleListByTriangleList.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.

###### Preview

-   Triangle and Square by Triangle List

    ![triangleAndSquareByTriangleList][triangle-and-square-by-triangle-list-image]

[//]: # "Image declaration"
[triangle-and-square-by-triangle-list-image]: ./preview/triangleAndSquareByTriangleList.png "Triangle and Square by Triangle List"
