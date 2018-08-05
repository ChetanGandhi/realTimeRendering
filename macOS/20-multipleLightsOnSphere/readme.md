# Multiple Lights On Sphere

###### How to compile

```
mkdir -p multipleLightsOnSphere.app/Contents/MacOS

clang++ -o multipleLightsOnSphere.app/Contents/MacOS/multipleLightsOnSphere *.mm *.cpp -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `1` to `9` key to increase the speed.
-   Press `f` key to toggle fullscreen mode.
-   Press `a` key to toggle animation.
-   Press `l` key to toggle light.

###### Preview

-   Light Disabled

    ![lightsDisabled][lights-disabled-image]

-   Light Enabled

    ![lightsEnabled][lights-enabled-image]

[//]: # "Image declaration"
[lights-disabled-image]: ./preview/lightsDisabled.png "Lights Disabled"
[lights-enabled-image]: ./preview/lightsEnabled.png "Lights Enabled"
