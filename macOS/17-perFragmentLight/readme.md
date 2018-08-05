# Per Fragment Light

###### How to compile

-   Compile the program.

```
mkdir -p perFragmentLight.app/Contents/MacOS

clang++ -o perFragmentLight.app/Contents/MacOS/perFragmentLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `F` or `f` to toggle full-screen mode.
-   Press `l` key to enable lighting.

###### Preview

-   Per Fragment Light Disabled

    ![perFragmentLightDisabled][per-vertex-light-disabled-image]

-   Per Fragment Light Enabled

    ![perFragmentLightEnabled][per-vertex-light-enabled-image]

[//]: # "Image declaration"
[per-vertex-light-disabled-image]: ./preview/perFragmentLightDisabled.png "Per Fragment Light Disabled"
[per-vertex-light-enabled-image]: ./preview/perFragmentLightEnabled.png "Per Fragment Light Enabled"
