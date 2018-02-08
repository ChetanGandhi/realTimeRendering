Fullscreen Center Window By GetSystemMetrics
============================================

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
cl.exe /EHsc /DUNICODE /Zi fullscreenCenterWindowByGetSystemMetrics.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![fullscreenCenterWindowByGetSystemMetrics][fullscreenCenterWindowByGetSystemMetrics-image]

[//]: # "Image declaration"

[fullscreenCenterWindowByGetSystemMetrics-image]: ./preview/fullscreenCenterWindowByGetSystemMetrics.png "Fullscreen Center Window By GetSystemMetrics"
