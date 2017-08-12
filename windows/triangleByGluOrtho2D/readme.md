Triangle by GluOrtho2D
===================

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
cl.exe /EHsc /DUNICODE /Zi triangleByGluOrtho2D.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![triangleByGluOrtho2D][triangleByGluOrtho2D-image]

<!-- Image declaration -->

[triangleByGluOrtho2D-image]: ./preview/triangleByGluOrtho2D.png "OpenGL Triangle By GluOrtho2D"
