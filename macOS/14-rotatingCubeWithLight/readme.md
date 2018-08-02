# Rotating Cube with Light

###### How to compile

-   Compile the program.

```
mkdir -p rotatingCubeWithLight.app/Contents/MacOS

clang -o rotatingCubeWithLight.app/Contents/MacOS/rotatingCubeWithLight -.m -.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `F` or `f` to toggle full-screen mode.
-   Press `1` to `9` key to increase the speed.
-   Press `l` key to enable lighting.
-   Press `a` key to enable animation.

###### Preview

-   Rotating cube with light disabled

    ![rotatingCubeWithLightDisabled][rotating-cube-with-light-disabled-image]

-   Rotating cube with light enabled

    ![rotatingCubeWithLightEnabled][rotating-cube-with-light-enabled-image]

[//]: # "Image declaration"
[rotating-cube-with-light-disabled-image]: ./preview/rotatingCubeWithLightDisabled.png "Rotating Cube with Light Disabled"
[rotating-cube-with-light-enabled-image]: ./preview/rotatingCubeWithLightEnabled.png "Rotating Cube with Light Enabled"
