Triangle by GluPerspective
=====================

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

<!-- Image declaration -->

[triangleByGluPerspective-image]: ./preview/triangleByGluPerspective.png "OpenGL Triangle By GluPerspective"
