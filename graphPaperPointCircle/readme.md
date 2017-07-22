Point Circle On Graph Paper
===========================

###### How to compile

- First compile the resource file.

```
cd resource
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi pointCircle.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![pointCircle][pointCircle-image]

<!-- Image declaration -->

[pointCircle-image]: ./preview/pointCircle.png "Point Circle on Graph Paper"
