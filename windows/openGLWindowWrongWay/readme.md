OpenGL Window Single Buffer | The Wrong Way
===========================================

This is not the correct way to implement the OpenGL rendering logic. The call to ```display``` should not be in game loop instead of in ```WM_PATNT```.

This is because rendering done by ```WM_PATNT``` is single buffered, single threaded and also graphics state is not saved. Hence this is very slow and not applicable for games, where faster rendering is required. Due to single buffered, screen tearing issue is observed.

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi openGLWindowSingleBuffer.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
- OpenGL Window
![openGLWindowSingleBuffer][openGLWindowSingleBuffer-image]

<!-- Image declaration -->

[openGLWindowSingleBuffer-image]: ./preview/openGLWindowSingleBuffer.png "OpenGL Window"
