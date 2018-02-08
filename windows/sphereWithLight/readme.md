Sphere With Light
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
cl.exe /EHsc /DUNICODE /Zi sphereWithLight.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```l``` key to toggle lighting.

###### Preview
- Lighting enabled
![sphereWithLightOn][sphereWithLightOn-image]

- Lighting disabled
![sphereWithLightOff][sphereWithLightOff-image]

[//]: # "Image declaration"

[sphereWithLightOn-image]: ./preview/sphereWithLightOn.png "Sphere with light on"
[sphereWithLightOff-image]: ./preview/sphereWithLightOff.png "Sphere with light off"
