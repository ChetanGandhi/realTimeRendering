# Empty Window

###### How to compile

*   Compile the program.

```
mkdir -p window.app/Contents/MacOS

clang -o window.app/Contents/MacOS/window *.m -framework Cocoa
```

###### Keyboard Shortcuts

*   Press `Esc` key to quit.
*   Press `F` or `f` to toggle full-screen mode.
*   Press `left` or `right` mouse button to show corresponding message.

###### Preview

*   Window
    ![window][window-image]

*   Left mouse click
    ![leftMouseClick][left-mouse-click-image]

*   Right mouse click
    ![rightMouseClick][right-mouse-click-image]

*   Fullscreen
    ![fullscreen][fullscreen-image]

[//]: # "Image declaration"
[window-image]: ./preview/window.png "First Window"
[left-mouse-click-image]: ./preview/leftMouseClick.png "Left Mouse Click"
[right-mouse-click-image]: ./preview/rightMouseClick.png "Right Mouse Click"
[fullscreen-image]: ./preview/fullscreen.png "Fullscreen"
