# Triangle Ortho

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
cl.exe /EHsc /DUNICODE /Zi triangleOrtho.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib d3dcompiler.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.

###### Preview

-   Triangle Ortho

    ![triangleOrtho][triangle-ortho-image]

[//]: # "Image declaration"
[triangle-ortho-image]: ./preview/triangleOrtho.png "Triangle Ortho"
