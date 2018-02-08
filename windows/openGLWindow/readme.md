OpenGL Window
============

This is the bare bone code to get started with OpenGL. The ```display``` function is properly implemented and required ```PFD_DOUBLEBUFFER``` flag is also set to so that OpenGL start using double buffers for rendering.

The ```SwapBuffers``` is an Win32 API, as we are in fixed-function OpenGL.

###### How to compile

- First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi openGLWindow.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
- OpenGL Window
![openGLlWindow][openGLWindow-image]

[//]: # "Image declaration"

[openGLWindow-image]: ./preview/openGLWindow.png "OpenGL Window"
