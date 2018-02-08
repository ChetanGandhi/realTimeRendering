Square
======

In ```resize``` we need to set proper left and right or top or bottom as per the ration of width and height so that the square is shown as a "square" when window is resized.

```c++

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    if(width == 0)
    {
        width = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = 1.0f;

    if(width <= height)
    {
        ratio = (GLfloat)height / (GLfloat)width;
        glOrtho(-50.0f, 50.0f, -50.0f * ratio, 50.0f * ratio, -50.0f, 50.0f);
    }
    else
    {
        ratio = (GLfloat)width / (GLfloat)height;
        glOrtho(-50.0f * ratio, 50.0f * ratio, -50.0f, 50.0f, -50.0f, 50.0f);
    }
}

```

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
cl.exe /EHsc /DUNICODE /Zi square.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![square][square-image]

[//]: # "Image declaration"

[square-image]: ./preview/square.png "OpenGL Square"
