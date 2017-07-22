Fullscreen Window
============

###### How to compile

- First compile the resource file.

```
cd resource
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi fullscreenWindow.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
- Normal Mode
![normalWindow][normalWindow-image]

- Fullscreen Mode
![fullscreenWindow][fullscreenWindow-image]

<!-- Image declaration -->

[normalWindow-image]: ./preview/normalWindow.png "Normal Window"
[fullscreenWindow-image]: ./preview/fullscreenWindow.png "Fullscreen Window"
