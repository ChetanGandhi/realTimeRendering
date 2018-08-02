# Smiley Tweaked

###### How to compile

-   Compile the program.

```
mkdir -p smileyTweaked.app/Contents/MacOS

clang -o smileyTweaked.app/Contents/MacOS/smileyTweaked *.m *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `F` or `f` to toggle full-screen mode.
-   Press `1` key to show bottom left quadrant of texture.
-   Press `2` key to show full texture.
-   Press `3` key to repeat the texture 2 times.
-   Press `4` key to show center of texture.
-   Press any other key to remove the texture and show white square.

###### Preview

-   No Texture

    ![whiteSquare][white-square-image]

-   Bottom Left Quadrant of Texture

    ![bottomLeftQuadrant][bottom-left-quadrant-image]

-   Full Texture

    ![fullTexture][full-texture-image]

-   Repeating texture

    ![repeatTexture][repeat-texture-image]

-   Center of Texture

    ![centerOfTexture][center-of-texture-image]

[//]: # "Image declaration"
[white-square-image]: ./preview/whiteSquare.png "White Square"
[bottom-left-quadrant-image]: ./preview/bottomLeftQuadrant.png "Bottom Left Quadrant"
[full-texture-image]: ./preview/fullTexture.png "Full Texture"
[repeat-texture-image]: ./preview/repeatTexture.png "Repeat Texture"
[center-of-texture-image]: ./preview/centerOfTexture.png "Center Of Texture"
