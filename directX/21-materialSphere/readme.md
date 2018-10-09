# Material Sphere

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file
-   The `Sphere.lib` and `Sphere.dll` is provided by Sir and is not included with this source. You can draw any other shape.
-   As we are using x86 version of `Sphere.lib` and `Sphere.dll`, we have to use x86 version of all other dll's and lib's.

```
cl.exe /EHsc /DUNICODE /Zi /I %SPHERE_PATH% materialSphere.cpp /link resources\resource.res /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib Sphere.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `1` to `9` key to increase the speed.
-   Press `f` key to toggle fullscreen mode.
-   Press `a` key to toggle animation.
-   Press `l` key to toggle light.
-   Press `x` key to rotate light on x-axis.
-   Press `y` key to rotate light on y-axis.
-   Press `z` key to rotate light on z-axis.

###### Preview

-   Light Disabled

    ![lightsDisabled][lights-disabled-image]

-   Light Enabled

    ![lightsEnabled][lights-enabled-image]

-   Light Enabled - Rotation on X-Axis

    ![lightsEnabled][lights-enabled-x-axis-image]

-   Light Enabled - Rotation on Y-Axis

    ![lightsEnabled][lights-enabled-y-axis-image]

-   Light Enabled - Rotation on Z-Axis

    ![lightsEnabled][lights-enabled-z-axis-image]

[//]: # "Image declaration"
[lights-disabled-image]: ./preview/lightsDisabled.png "Lights Disabled"
[lights-enabled-image]: ./preview/lightsEnabled.png "Lights Enabled"
[lights-enabled-x-axis-image]: ./preview/lightsEnabledXAxis.png "Light Enabled - Rotation on X-Axis"
[lights-enabled-y-axis-image]: ./preview/lightsEnabledYAxis.png "Light Enabled - Rotation on Y-Axis"
[lights-enabled-z-axis-image]: ./preview/lightsEnabledZAxis.png "Light Enabled - Rotation on Z-Axis"
