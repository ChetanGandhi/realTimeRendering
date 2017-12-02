Multiple Lights on Sphere
=========================

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
cl.exe /EHsc /DUNICODE /Zi multipleLightsOnSphere.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```l``` key to toggle lighting.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview

- Lighting disabled
![sphereLightOff][sphereLightOff-image]

- Lighting enabled
![sphereLightOn][sphereLightOn-image]

<!-- Image declaration -->

[sphereLightOff-image]: ./preview/sphereLightOff.png "Sphere with light off"
[sphereLightOn-image]: ./preview/sphereLightOn.png "Sphere with light on"
