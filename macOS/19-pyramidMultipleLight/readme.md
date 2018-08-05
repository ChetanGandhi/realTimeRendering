# Pyramid with Multiple Light

###### How to compile

```
mkdir -p pyramidWithMultipleLight.app/Contents/MacOS

clang++ -o pyramidWithMultipleLight.app/Contents/MacOS/pyramidWithMultipleLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `1` to `9` key to increase the speed.
-   Press `f` key to toggle fullscreen mode.
-   Press `a` key to toggle animation.
-   Press `l` key to toggle lighting.

###### Preview

-   Pyramid Light Disabled

    ![pyramidLightDisabled][pyramid-light-disabled-image]

-   Pyramid Light Enabled

    ![pyramidLightEnabled][pyramid-light-enabled-image]

[//]: # "Image declaration"
[pyramid-light-disabled-image]: ./preview/pyramidLightDisabled.png "Pyramid Light Disabled"
[pyramid-light-enabled-image]: ./preview/pyramidLightEnabled.png "Pyramid Light Enabled"
