View Transformation by gluLookAt
================================

Instead of moving the modal, we can move the viewer so that the triangle is shown properly. To move viewer use ```glTranslatef``` with positive z value or use ```gluLookAt```. We are going to use ```gluLookAt```.

```gluLookAt``` have 9 parameters, of which first 3 parameters are called as eye vector, next 3 parameters are called as center vector and last 3 parameters are called as up vector.

In following code, we are stating that the viewer is out of screen ```0.0f, 0.0f, 6.0f``` i.e. viewer is at 6 unit on z-axis. The viewer is looking at ```0.0f, 0.0f, 0.0f``` i.e at center of the screen and its up is ```0.0f, 1.0f, 0.0f``` i.e y-axis.

```c++

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
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
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi viewTransformationByGluLookAt.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib glu32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.

###### Preview
![viewTransformationByGluLookAt][viewTransformationByGluLookAt-image]

<!-- Image declaration -->

[viewTransformationByGluLookAt-image]: ./preview/viewTransformationByGluLookAt.png "OpenGL Modal Transformation"
