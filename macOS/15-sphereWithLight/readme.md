# Sphere with Light

###### How to compile

-   Compile the program.

```
mkdir -p sphereWithLight.app/Contents/MacOS

clang++ -o sphereWithLight.app/Contents/MacOS/sphereWithLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `F` or `f` to toggle full-screen mode.
-   Press `l` key to enable lighting.

###### Preview

-   Sphere with light disabled

    ![sphereWithLightDisabled][rotating-cube-with-light-disabled-image]

-   Sphere with light enabled

    ![sphereWithLightEnabled][rotating-cube-with-light-enabled-image]

[//]: # "Image declaration"
[rotating-cube-with-light-disabled-image]: ./preview/sphereWithLightDisabled.png "Sphere with Light Disabled"
[rotating-cube-with-light-enabled-image]: ./preview/sphereWithLightEnabled.png "Sphere with Light Enabled"
