Modal Transformation
====================

In ```resize``` we had to change the angle from ```45.0f``` to ```0.0f``` so that the triangle was visible. To avoid this, we will have to translate the modal deep into the screen so that the triangle is visible.

Also remove the call to ```glLoadIdentity``` from ```drawTriangle``` as we need to draw triangle at translated position. The ```drawTriangle``` should now be used to draw the triangle at given position.

```c++

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -6.0f);
    drawTriangle();
    SwapBuffers(hdc);
}

void drawTriangle(void)
{
    glBegin(GL_TRIANGLES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glEnd();
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
cl.exe /EHsc /DUNICODE /Zi modalTransformation.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![modalTransformation][modalTransformation-image]

[//]: # "Image declaration"

[modalTransformation-image]: ./preview/modalTransformation.png "OpenGL Modal Transformation"
