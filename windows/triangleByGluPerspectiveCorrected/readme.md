Triangle by GluPerspective | Corrected
======================================

The angle used was 45 degree, which was not right, setting it to 0 degree makes the triangle visible.

###### How to compile

- First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi triangleByGluPerspective.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![triangleByGluPerspective][triangleByGluPerspective-image]

[//]: # "Image declaration"

[triangleByGluPerspective-image]: ./preview/triangleByGluPerspective.png "OpenGL Triangle By GluPerspective"
