OpenGL Window | The Wrong Way
============

This is not the correct way to implement the OpenGL rendering logic. The call to ```display``` should not be in game loop instead of in ```WM_PATNT```.

This is because rendering done by ```WM_PATNT``` is single buffered, single threaded and also graphics state is not saved. Hence this is very slow and not applicable for games, where faster rendering is required. Due to single buffered, screen tearing issue is observed.

###### How to compile

```
cl.exe /EHsc /D:UNICODE /DEBUG openGLWindow.cpp user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
- OpenGL Window
![openGLlWindow][openGLWindow-image]

<!-- Image declaration -->

[openGLWindow-image]: ./preview/openGLWindow.png "OpenGL Window"
