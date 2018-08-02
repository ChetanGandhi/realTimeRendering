# OpenGL Window

###### How to compile

*   Compile the program.

```
mkdir -p openGLWindow.app/Contents/MacOS

clang -o openGLWindow.app/Contents/MacOS/openGLWindow *.m *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
```

###### Keyboard Shortcuts

*   Press `Esc` key to quit.
*   Press `F` or `f` to toggle full-screen mode.

###### Preview

*   OpenGL Window
    ![openGLWindow][opengl-window-image]

[//]: # "Image declaration"
[opengl-window-image]: ./preview/openGLWindow.png "OpenGL Window"
