Robot Arm By GLUT
=================

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
cl.exe /EHsc /DUNICODE /Zi /I %FREEGLUT_PATH%\include robotArm.cpp /link resources\resource.res /LIBPATH:%FREEGLUT_PATH%\lib\x64 user32.lib kernel32.lib gdi32.lib openGL32.lib freeglut.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```S``` key to rotate the shoulder in clockwise direction.
- Press ```s``` key to rotate the shoulder in anti-clockwise direction.
- Press ```E``` key to rotate the elbow in clockwise direction.
- Press ```e``` key to rotate the elbow in anti-clockwise direction.

###### Preview
![robotArm][robotArm-image]

[//]: # "Image declaration"

[robotArm-image]: ./preview/robotArm.png "Robot Arm"
