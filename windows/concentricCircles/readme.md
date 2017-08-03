Concentric Circles
=======================

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi concentricCircles.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![concentricCircles][concentricCircles-image]

<!-- Image declaration -->

[concentricCircles-image]: ./preview/concentricCircles.png "Concentric Circles"
