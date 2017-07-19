Bordered Square On Graph Paper
================================

###### How to compile

- First compile the resource file.

```
rc.exe resource\resource.rc
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi borderedSquare.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![borderedSquare][borderedSquare-image]

<!-- Image declaration -->

[borderedSquare-image]: ./preview/borderedSquare.png "Bordered Square on Graph Paper"
