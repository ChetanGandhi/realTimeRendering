Cornflower Rectangle
====================

###### How to compile

- First compile the resource file.

```
rc.exe resource\resource.rc
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi cornflowerRectangle.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![cornflowerRectangle][cornflowerRectangle-image]

<!-- Image declaration -->

[cornflowerRectangle-image]: ./preview/cornflowerRectangle.png "OpenGL Cornflower Rectangle"
