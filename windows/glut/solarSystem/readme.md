Solar System By GLUT
====================

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
cl.exe /EHsc /DUNICODE /Zi /I %FREEGLUT_PATH%\include solarSystem.cpp /link resources\resource.res /LIBPATH:%FREEGLUT_PATH%\lib\x64 user32.lib kernel32.lib gdi32.lib openGL32.lib freeglut.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```Y``` key to revolve the earth in clockwise direction around the sun.
- Press ```y``` key to revolve the earth in anti-clockwise direction around the sun.
- Press ```D``` key to rotate the earth in clockwise direction.
- Press ```d``` key to rotate the earth in anti-clockwise direction.

###### Preview
![solarSystem][solarSystem-image]

[//]: # "Image declaration"

[solarSystem-image]: ./preview/solarSystem.png "Solar System"
