Point at Center
===============

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
cl.exe /EHsc /DUNICODE /Zi pointAtCenter.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` Key to set point side to 1.
- Press ```2``` Key to set point side to 2.
- Press ```3``` Key to set point side to 3.
- Press ```4``` Key to set point side to 4.
- Press ```5``` Key to set point side to 5.
- Press ```6``` Key to set point side to 6.
- Press ```7``` Key to set point side to 7.
- Press ```8``` Key to set point side to 8.
- Press ```9``` Key to set point side to 9.

###### Preview
![pointAtCenter][pointAtCenter-image]

<!-- Image declaration -->

[pointAtCenter-image]: ./preview/pointAtCenter.png "Point at Center"
