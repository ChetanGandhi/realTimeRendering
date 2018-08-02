# Smiley

###### How to compile

-   Compile the program.

```
mkdir -p smiley.app/Contents/MacOS

clang -o smiley.app/Contents/MacOS/smiley *.m *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

-   Press `Esc` key to quit.
-   Press `F` or `f` to toggle full-screen mode.

###### Preview

-   Smiley

    ![smiley][smiley-image]

[//]: # "Image declaration"
[smiley-image]: ./preview/smiley.png "Smiley"
