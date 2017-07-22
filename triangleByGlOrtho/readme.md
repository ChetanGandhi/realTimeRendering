Triangle by GlOrtho
===================

###### How to compile

- First compile the resource file.

```
cd resource
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi triangleByGlOrtho.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![triangleByGlOrtho][triangleByGlOrtho-image]

<!-- Image declaration -->

[triangleByGlOrtho-image]: ./preview/triangleByGlOrtho.png "OpenGL Triangle"
