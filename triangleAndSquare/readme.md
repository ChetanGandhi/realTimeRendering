Triangle and Square
===================

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi triangleAndSquare.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![triangleAndSquare][triangleAndSquare-image]

<!-- Image declaration -->

[triangleAndSquare-image]: ./preview/triangleAndSquare.png "OpenGL Modal Transformation"
