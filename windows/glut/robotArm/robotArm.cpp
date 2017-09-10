#define _USE_MATH_DEFINES

#include <GL/freeglut.h>
#include <cstdlib>
#include <math.h>

#include "resources/resource.h"

RECT windowRect = {0, 0, 800, 600};

bool isFullscreen = false;
bool isActive = false;
bool isEscapeKeyPressed = false;

GLint angleShoulder = 0;
GLint angleElbow = 0;

GLUquadric *guadric = NULL;

void initialize(void);
void cleanUp(void);
void display(void);
void drawRobotArm(void);
void resize(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(windowRect.right - windowRect.left ,windowRect.bottom - windowRect.top);
    glutCreateWindow("CG - GLUT Robot Arm");
    initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(cleanUp);
	glutMainLoop();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'E':
            angleElbow = (angleElbow + 3) % 360;
            glutPostRedisplay();
        break;

        case 'e':
            angleElbow = (angleElbow - 3) % 360;
            glutPostRedisplay();
        break;

        case 27:
            glutLeaveMainLoop();
        break;

        case 'F':
        case 'f':
        if(isFullscreen == false)
        {
            glutFullScreen();
            isFullscreen = true;
        }
        else
        {
            glutLeaveFullScreen();
            isFullscreen = false;
        }
        break;

        case 'S':
            angleShoulder = (angleShoulder + 3) % 360;
            glutPostRedisplay();
        break;

        case 's':
            angleShoulder = (angleShoulder - 3) % 360;
            glutPostRedisplay();
        break;

        default:
        break;
    }
}

void mouse(int button, int state, int x, int y)
{
	switch(button)
	{
    	case GLUT_LEFT_BUTTON:
		break;

        default:
		break;
	}
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
}

void initialize(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTranslatef(0.0f, 0.0f, -12.0f);

    // Push initial matrix.
    glPushMatrix();

    // Draw the robot arm.
    drawRobotArm();

    // Pop initial matrix.
    glPopMatrix();
    glutSwapBuffers();
}

void drawRobotArm(void)
{
    // Draw the shoulder
    glRotatef((GLfloat)angleShoulder, 0.0f, 0.0f, 1.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);

    // Push the shoulder transformation state.
    glPushMatrix();

    glScalef(2.0f, 0.5f, 1.0f);
    guadric = gluNewQuadric();
    glColor3f(0.5f, 0.35f, 0.05f);
    gluSphere(guadric, 0.5, 10, 10);

    // Pop back to shoulder transformation state.
    glPopMatrix();

    // Draw the elbow
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef((GLfloat)angleElbow, 0.0f, 0.0f, 1.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);

    // Push the elbow transformation state.
    glPushMatrix();
    glScalef(2.0f, 0.5f, 1.0f);
    guadric = gluNewQuadric();
    glColor3f(0.5f, 0.35f, 0.05f);
    gluSphere(guadric, 0.5, 10, 10);

    // Pop the elbow transformation state.
    glPopMatrix();
}

void cleanUp(void)
{
    if(isFullscreen)
    {
        glutLeaveFullScreen();
    }
}
