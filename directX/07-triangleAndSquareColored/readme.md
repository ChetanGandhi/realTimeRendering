# Triangle and Square Colored

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
cl.exe /EHsc /DUNICODE /Zi triangleAndSquareColored.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.

###### Preview

-   Triangle and Square

    ![Triangle and Square Colored][triangle-and-square-colored-image]

[//]: # "Image declaration"
[triangle-and-square-colored-image]: ./preview/triangleAndSquareColored.png "Triangle and Square Colored"
