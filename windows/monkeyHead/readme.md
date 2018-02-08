Monkey Head: Model Loading
==========================

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
cl.exe /EHsc /DUNICODE /Zi monkeyHead.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` to ```9``` key to increase the rotation speed.
- Press ```w``` to toggle wireframe mode.

###### Preview

- Monkey head wireframe without lights
![wireframeWithoutLights][wireframeWithoutLights-image]

- Monkey head wireframe with lights
![wireframeWithLights][wireframeWithLights-image]

- Monkey head fill without lights
![fillWithoutLights][fillWithoutLights-image]

- Monkey head fill with lights
![fillWithLights][fillWithLights-image]

[//]: # "Image declaration"

[fillWithLights-image]: ./preview/fillWithLights.png "Monkey head fill with lights"
[fillWithoutLights-image]: ./preview/fillWithoutLights.png "Monkey head fill without lights"
[wireframeWithLights-image]: ./preview/wireframeWithLights.png "Monkey head wireframe with lights"
[wireframeWithoutLights-image]: ./preview/wireframeWithoutLights.png "Monkey head wireframe without lights"
