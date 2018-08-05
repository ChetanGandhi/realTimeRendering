# Per Vertex Light

###### How to compile

-   Compile the program.

```
mkdir -p perVertexLight.app/Contents/MacOS

clang++ -o perVertexLight.app/Contents/MacOS/perVertexLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `F` or `f` to toggle full-screen mode.
-   Press `l` key to enable lighting.

###### Preview

-   Per Vertex Light Disabled

    ![perVertexLightDisabled][per-vertex-light-disabled-image]

-   Per Vertex Light Enabled

    ![perVertexLightEnabled][per-vertex-light-enabled-image]

[//]: # "Image declaration"
[per-vertex-light-disabled-image]: ./preview/perVertexLightDisabled.png "Per Vertex Light Disabled"
[per-vertex-light-enabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
