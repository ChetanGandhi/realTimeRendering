Pyramid And Cube Rotation
=========================

###### How to compile

- Compile the program.

```
g++ -o pyramidAndCubeRotation pyramidAndCubeRotation.cpp -lX11 -lGL -lGLU

```

###### Debug Logs

A debug log file is created showing the sequence of function calls.
```
---------- CG: OpenGL Debug Logs Start ----------
LOG: Entering main
LOG: Entering createWindow
LOG: Exiting createWindow
LOG: Entering initialize
LOG: Entering resize
LOG: Exiting resize
LOG: Exiting initialize
LOG: Entering handleEvent
LOG: Exiting handleEvent
LOG: Entering handleEvent
...

```
###### Keyboard Shortcuts
- Press ```Esc``` key to quit.
- Press ```F``` or ```f``` to toggle full-screen mode.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview
- Pyramid And Cube Rotation
![pyramidAndCubeRotation][pyramidAndCubeRotation-image]

[//]: # "Image declaration"

[pyramidAndCubeRotation-image]: ./preview/pyramidAndCubeRotation.png "Pyramid And Cube Rotation"
