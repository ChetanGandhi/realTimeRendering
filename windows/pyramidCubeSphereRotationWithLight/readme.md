Pyramid Cube Sphere Rotation with Light
=======================================

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
cl.exe /EHsc /DUNICODE /Zi pyramidCubeSphereRotationWithLight.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```l``` key to toggle lighting.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview

- Lighting disabled
![pyramidLightOff][pyramidLightOff-image]
![cubeLightOff][cubeLightOff-image]
![sphereLightOff][sphereLightOff-image]

- Lighting enabled
![pyramidLightOn][pyramidLightOn-image]
![cubeLightOn][cubeLightOn-image]
![sphereLightOn][sphereLightOn-image]

<!-- Image declaration -->

[pyramidLightOn-image]: ./preview/pyramidLightOn.png "Pyramid with light on"
[cubeLightOn-image]: ./preview/cubeLightOn.png "Cube with light on"
[sphereLightOn-image]: ./preview/sphereLightOn.png "Sphere rotation with light on"

[pyramidLightOff-image]: ./preview/pyramidLightOff.png "Pyramid with light off"
[cubeLightOff-image]: ./preview/cubeLightOff.png "Cubew ith light off"
[sphereLightOff-image]: ./preview/sphereLightOff.png "Sphere with light off"
