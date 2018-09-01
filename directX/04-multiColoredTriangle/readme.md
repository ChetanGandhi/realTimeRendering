# Multi Colored Triangle

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

-   Multi Colored Triangle

    ![multiColoredTriangle][multi-colored-triangle-image]

[//]: # "Image declaration"
[multi-colored-triangle-image]: ./preview/multiColoredTriangle.png "Multi Colored Triangle"
