Rectangle
========

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi rectangle.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
* Full size

![rectangle][rectangle-image]

* Small size

    As the full size rectangle covers full window, this is a small size preview.

![rectangleSmall][rectangleSmall-image]

<!-- Image declaration -->

[rectangle-image]: ./preview/rectangle.png "OpenGL Rectangle Small"
[rectangleSmall-image]: ./preview/rectangleSmall.png "OpenGL Rectangle Small"
