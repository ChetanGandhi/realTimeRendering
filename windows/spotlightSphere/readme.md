Spotlight on Sphere
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
cl.exe /EHsc /DUNICODE /Zi spotlightSphere.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```l``` key to toggle lighting.
- Press ```0``` key to use light 0.
- Press ```1``` key to use light 1, i.e. spotlight.

###### Preview

- Lighting disabled
![lightOff][lightOff-image]

- Lighting enabled
![lightOn][lightOn-image]

- Spotlight enabled
![spotlightOn][spotlightOn-image]

[//]: # "Image declaration"

[lightOn-image]: ./preview/lightOn.png "Sphere with light on"
[spotlightOn-image]: ./preview/spotlightOn.png "Sphere with spotlight on"
[lightOff-image]: ./preview/lightOff.png "Sphere with light off"
