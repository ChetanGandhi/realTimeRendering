# Tessellation Shader with Patch Parameter

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file and glew32.lib

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include tessellationShaderWithPatchParameter.cpp.cpp /link resources\resource.res /LIBPATH:%GLEW_LIB_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib glew32.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.
-   Press `Up Arrow` key to increase the number of line segments.
-   Press `Down Arrow` key to decrease the number of line segments.

###### Preview

-   Line segment

    ![lineSegment][line-segment-image]

-   Curve

    ![curve][curve-image]

[//]: # "Image declaration"
[line-segment-image]: ./preview/lineSegment.png "Line Segment"
[curve-image]: ./preview/curve.png "Curve"
