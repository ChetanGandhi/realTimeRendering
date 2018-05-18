# Multiple Lights On Sphere

###### How to compile

*   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

*   Now compile the program.

```
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include /I %SPHERE_PATH% multipleLightsOnSphere.cpp /link resources\resource.res /LIBPATH:%GLEW32_LIB_PATH% /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glew32.lib Sphere.lib
```

###### Keyboard shortcuts

*   Press `Esc` key to quit.
*   Press `1` to `9` key to increase the speed.
*   Press `f` key to toggle fullscreen mode.
*   Press `a` key to toggle animation.
*   Press `l` key to toggle light.

###### Preview

*   Light Disabled

    ![lightsDisabled][lights-disabled-image]

*   Light Enabled

    ![lightsEnabled][lights-enabled-image]

[//]: # "Image declaration"
[lights-disabled-image]: ./preview/lightsDisabled.png "Lights Disabled"
[lights-enabled-image]: ./preview/lightsEnabled.png "Lights Enabled"
