Deathly Hallows Rotation
========================

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi deathlyHallowsRotation.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` to ```9``` key to increase the rotation speed.

###### Preview
![deathlyHallowsRotation][deathlyHallowsRotation-image]

<!-- Image declaration -->

[deathlyHallowsRotation-image]: ./preview/deathlyHallowsRotation.png "Deathly Hallows Rotation"
