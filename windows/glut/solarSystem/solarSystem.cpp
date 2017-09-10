#define _USE_MATH_DEFINES

#include <GL/freeglut.h>
#include <cstdlib>
#include <math.h>

#include "resources/resource.h"

RECT windowRect = {0, 0, 800, 600};

bool isFullscreen = false;

GLint year = 0;
GLint day = 0;

GLUquadric *guadric = NULL;

void initialize(void);
void cleanUp(void);
void display(void);
void drawSolarSystem(void);
void resize(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(windowRect.right - windowRect.left ,windowRect.bottom - windowRect.top);
    glutCreateWindow("CG - GLUT Solar System");
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

        case 'D':
            day = (day + 6) % 360;
            glutPostRedisplay();
            break;

            case 'd':
            day = (day - 3) % 360;
            glutPostRedisplay();
            break;

            case 'y':
            year = (year + 3) % 360;
            glutPostRedisplay();
            break;

            case 'Y':
            year = (year - 3) % 360;
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
    gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Draw the solar system.
    drawSolarSystem();

    glutSwapBuffers();
}

void drawSolarSystem(void)
{
    // Push initial matrix.
    glPushMatrix();

    // Rotate the sphere by 90 degree on x-axis
    // because sphere created by GLUT has it rotated in x-axis
    // The north-south poles are on x-axis instead of y-axis.
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw the sun
    guadric = gluNewQuadric();
    glColor3f(1.0f, 1.0f, 0.0f);

    // 3rd parameter is for slices (like longitudes)
    // 4th parameter is for stacks (like latitudes)
    // Higher the value of 3rd and 4th parameters, i.e. more the subdivisions,
    // more circular the sphere will look.
    gluSphere(guadric, 0.75, 30, 30);

    // Pop back to initial state.
    glPopMatrix();

    // Draw the earth
    glPushMatrix();

    glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
    glTranslatef(1.5f, 0.0f, 0.0f);

    // Rotate the sphere by 90 degree on x-axis
    // because sphere created by GLUT has it rotated in x-axis
    // The north-south poles are on x-axis instead of y-axis.
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    // Rotate the earth by days on z-axis as we have rotated on x-axis.
    glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    guadric = gluNewQuadric();
    glColor3f(0.4f, 0.9f, 1.0f);
    gluSphere(guadric, 0.2f, 20, 20);

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
