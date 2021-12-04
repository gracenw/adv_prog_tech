#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <GL/gl.h>
#include <GL/glut.h>
#include <glm.hpp>

using namespace std;

void threadFunction(ECE_UAV * pUAV);

static double fieldWidth = 48.80;
static double fieldLength = 91.44;

/* track number of teapots that have come within 10m of (0, 0, 50) after 60 seconds */
atomic_uint64_t endCondition(0);

/* struct to easily access positional data */
struct triple
{
    double x;
    double y;
    double z;
};

/* 
  class representating a teapot UAv
  holds current positional data of UAV as well as the controlling thread
*/
class ECE_UAV 
{
    public:
        /* constructor */
        ECE_UAV();

        /* member variables */
        double mass = 1;
        triple pos, vel, acc;
        thread thr;

        /* member functions */
        thread start()
        {
            return thread(threadFunction, this);
        }
};

void threadFunction(ECE_UAV * pUAV)
{
    /* wait 5 seconds at beginning of simulation */
    this_thread::sleep_for(chrono::seconds(5));

    /* loop until 60 seconds have passed and all teapots have met end condition */
    while (endCondition < 15)
    {
        /* update position, velocity, acceleration with kinematic equations */


        /* check for end condition */
        

        /* thread sleeps for 10ms */
        this_thread::sleep_for (chrono::milliseconds(10));
    }
}

void glInit() 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    /* draw teapot as threads handle updating positional data */
    glLoadIdentity();
    // loadObj("teapot.obj", );

    /* draw field */
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -4.0f);

    glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
    glEnd();

    glLoadIdentity();
    glutSwapBuffers();
}
 
void reshape(GLsizei width, GLsizei height) 
{
    // Compute aspect ratio of the new window
    if (height == 0) 
    {
        height = 1;
    }
    double aspect = (double) width / (double) height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity(); // Reset
    gluPerspective(45.0f, aspect, 0.1f, 100.0f); // Enable perspective projection with fovy, aspect, zNear and zFar
}

int main (int argc, char* argv[]) 
{
    /* initialize teapot objects */
    const unsigned numTeapots = 15;
    ECE_UAV teapots[numTeapots];

    /* set initial positional vectors */
    double currentY = -(fieldLength / 2);
    for (int i = 0; i < numTeapots; i++)
    {
        /* x positions */
        if (i < 5)
        {
            teapots[i].pos.x = fieldWidth / 2;
        }
        else if (i < 10)
        {
            teapots[i].pos.x = 0;
        }
        else 
        {
            teapots[i].pos.x = -(fieldWidth / 2);
        }

        /* y positions */
        teapots[i].pos.y = currentY;
        if (currentY == (fieldLength / 2))
        {
            currentY = -(fieldLength / 2);
        }
        else
        {
            currentY += (fieldLength / 4);
        }
        
        /* z positions */
        teapots[i].pos.z = 0;
    }
    



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("UAV Simulation");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glInit();
    glutMainLoop();

    /* launch threads with threadFunction */
    for (int i = 0; i < numTeapots; i ++)
    {
        teapots[i].thr = teapots[i].start();
    }

    /* join all threads */
    for (int i = 0; i < numTeapots; i ++)
    {
        teapots[i].thr.join();
    }

    /* return successfully */
    return 0;
}