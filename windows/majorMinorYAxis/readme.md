Major and Minor Y Axis
======================

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
cl.exe /EHsc /DUNICODE /Zi majorMinorYAxis.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![majorMinorYAxis][majorMinorYAxis-image]

[//]: # "Image declaration"

[majorMinorYAxis-image]: ./preview/majorMinorYAxis.png "Major and Minor Y Axis"
