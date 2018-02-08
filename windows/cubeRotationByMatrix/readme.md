Cube Rotation by Matrix
=======================

In this program, we are not using any transformation APIs provided by OpenGL. We are defining our own matrices to set identity, translate, scale and rotate the cube.

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
cl.exe /EHsc /DUNICODE /Zi cubeRotationByMatrix.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview
![cubeRotationByMatrix][cubeRotationByMatrix-image]

[//]: # "Image declaration"

[cubeRotationByMatrix-image]: ./preview/cubeRotationByMatrix.png "Cube Rotation by Matrix"
