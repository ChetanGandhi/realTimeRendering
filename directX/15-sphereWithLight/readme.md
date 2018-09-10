# Sphere with Light

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
cl.exe /EHsc /DUNICODE /Zi /I %SPHERE_PATH% sphereWithLight.cpp /link resources\resource.res /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib Sphere.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.
-   Press `l` key to enable lighting.

###### Preview

-   Sphere with Light Disabled

    ![sphereWithLightDisabled][sphere-with-light-disabled-image]

-   Sphere with Light Enabled

    ![sphereWithLightEnabled][sphere-with-light-enabled-image]

[//]: # "Image declaration"
[sphere-with-light-disabled-image]: ./preview/sphereWithLightDisabled.png "Sphere with Light Disabled"
[sphere-with-light-enabled-image]: ./preview/sphereWithLightEnabled.png "Sphere with Light Enabled"
