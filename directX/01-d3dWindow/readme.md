# DirectX Window

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file

```
cl.exe /EHsc /DUNICODE /Zi d3dWindow.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.

###### Preview

-   DirectX Window

    ![direct-x-window][direct-x-window-image]

[//]: # "Image declaration"
[direct-x-window-image]: ./preview/directXWindow.png "DirectX Window"
