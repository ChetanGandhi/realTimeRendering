Multi Colored Triangle
======================

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi multiColoredTriangle.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![multiColoredTriangle][multiColoredTriangle-image]

<!-- Image declaration -->

[multiColoredTriangle-image]: ./preview/multiColoredTriangle.png "OpenGL Multi Colored Triangle"
