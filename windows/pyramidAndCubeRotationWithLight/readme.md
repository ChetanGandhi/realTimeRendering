Pyramid and Cube Rotation: Lights and Normal
============================================

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
cl.exe /EHsc /DUNICODE /Zi pyramidAndCubeRotationWithLight.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```l``` key to toggle lighting.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview
- Lighting enabled
![pyramidAndCubeRotationWithLightOn][pyramidAndCubeRotationWithLightOn-image]

- Lighting disabled
![pyramidAndCubeRotationWithLightOff][pyramidAndCubeRotationWithLightOff-image]

[//]: # "Image declaration"

[pyramidAndCubeRotationWithLightOn-image]: ./preview/pyramidAndCubeRotationWithLightOn.png "Pyramid and Cube rotation with light on"
[pyramidAndCubeRotationWithLightOff-image]: ./preview/pyramidAndCubeRotationWithLightOff.png "Pyramid and Cube rotation with light off"
