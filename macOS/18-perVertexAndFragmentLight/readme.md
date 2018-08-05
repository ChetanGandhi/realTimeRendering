# Per Vertex and Fragment Light

###### How to compile

-   Compile the program.

```
mkdir -p perVertexAndFragmentLight.app/Contents/MacOS

clang++ -o perVertexAndFragmentLight.app/Contents/MacOS/perVertexAndFragmentLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `t` key to toggle fullscreen mode.
-   Press `l` key to enable lighting.
-   Press `v` key to enable per vertex lighting.
-   Press `f` key to enable per fragment lighting.

###### Preview

-   Light Disabled

    ![lightDisabled][light-disabled-image]

-   Per Vertex Lighting Enabled

    ![perVertexLightEnabled][per-vertex-light-enabled-image]

-   Per Fragment Lighting Enabled

    ![perFragmentLightEnabled][per-fragment-light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light Disabled"
[per-vertex-light-enabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
[per-fragment-light-enabled-image]: ./preview/perFragmentLightEnabled.png "Per Fragment Light Enabled"
