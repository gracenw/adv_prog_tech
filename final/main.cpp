#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <glm.hpp>

using namespace std;

/* declaring up here so ECE_UAV::start() can find it */
void threadFunction(ECE_UAV * pUAV);

/* generally useful static global variables */
static double fieldWidth = 48.80;
static double fieldLength = 91.44;
static const unsigned numTeapots = 15;
static unsigned k = 1;

/* threading variables */
mutex swap_mtx;
atomic_uint64_t endAll(0);

/* clock starting time point to compute time elapsed */
chrono::system_clock::time_point start;

/* struct to easily access positional data */
struct triple
{
    double x;
    double y;
    double z;
} sphereCenter{0, 0, 50}, origin{ 0, 0, 0 };

/* class representating a teapot UAV */
class ECE_UAV 
{
    public:
        /* constructor */
        ECE_UAV();

        /* member variables */
        double mass = 1;
        bool end = false;
        int index;
        triple pos, vel, acc;
        thread thr;

        /* member functions */
        thread start()
        {
            return thread(threadFunction, this);
        }
};

/* simple function to find the magnitude of a 3D vector */
inline double magnitude3D (triple * left, triple * right)
{
    return ( sqrt( pow((left->x - right->x), 2) + pow((left->y - right->y), 2) + pow((left->z - right->z), 2) ) );
}

/* sets new positional data with UAV pointer */
void getNewPos (ECE_UAV * pUAV)
{
    chrono::duration<double> elapsed = chrono::system_clock::now() - start;
    pUAV->pos.x = pUAV->pos.x + pUAV->vel.x * elapsed.count() + 0.5 * pUAV->acc.x * pow(elapsed.count(), 2);
    pUAV->pos.y = pUAV->pos.y + pUAV->vel.y * elapsed.count() + 0.5 * pUAV->acc.y * pow(elapsed.count(), 2);
    pUAV->pos.z = pUAV->pos.z + pUAV->vel.z * elapsed.count() + 0.5 * pUAV->acc.z * pow(elapsed.count(), 2);
}

/* sets new velocity values with UAV pointer */
void getNewVel (ECE_UAV * pUAV, bool twoMSCondition = false)
{
    chrono::duration<double> elapsed = chrono::system_clock::now() - start;
    pUAV->vel.x = pUAV->vel.x + pUAV->acc.x * elapsed.count();
    pUAV->vel.y = pUAV->vel.y + pUAV->acc.y * elapsed.count();
    pUAV->vel.z = pUAV->vel.z + pUAV->acc.z * elapsed.count();

    if (twoMSCondition)
    {
        while (magnitude3D(&pUAV->vel, &origin) > 2)
        {
            // FINISH
        }
    }
}

/* sets new acceleration values with UAV pointer */
void getNewAcc (ECE_UAV * pUAV, triple * directionalVector = NULL)
{
    /* determine magnitude of gravity and sphere force */
    double gravityForceZ = -10;
    double forceMagnitude = k * (10 - (magnitude3D(&pUAV->pos, &sphereCenter)));
    
    triple force;
    if (directionalVector == NULL)
    {
        /* calculate force of sphere vector using a random unit vector and magnitude */
        triple randomVector { rand() % 100, rand() % 100, rand() % 100 };
        double randMagnitude = magnitude3D(&randomVector, &origin);
        randomVector.x /= randMagnitude;
        randomVector.y /= randMagnitude;
        randomVector.z /= randMagnitude;

        /* calculate final total force of sphere (modeled as spring) and gravity */
        force.x = randomVector.x * forceMagnitude;
        force.y = randomVector.y * forceMagnitude;
        force.z = randomVector.z * forceMagnitude + gravityForceZ;
    }
    else
    {
        /* calculate final total force of sphere (modeled as spring) and gravity */
        force.x = directionalVector->x * forceMagnitude;
        force.y = directionalVector->y * forceMagnitude;
        force.z = directionalVector->z * forceMagnitude + gravityForceZ;
    }

    /* check that magnitude of force is less than 20 */
    while (magnitude3D(&force, &origin) > 20)
    {
        // FINISH
    }

    /* assign new acceleration using f = ma, m = 1 */
    pUAV->acc.x = force.x;
    pUAV->acc.y = force.y;
    pUAV->acc.z = force.z;
}

/* initialize teapot objects here so collision conditions can be checked - will be declared in main */
ECE_UAV teapots[numTeapots];

/* function called by each UAV thread to update location every 10ms */
void threadFunction (ECE_UAV * pUAV)
{
    /* wait 5 seconds at beginning of simulation */
    this_thread::sleep_for(chrono::seconds(5));

    /* create initial unit vector for movement towards sphere center */
    double distanceToSphere = magnitude3D(&pUAV->pos, &sphereCenter);
    triple directionToSphere 
    {
        (sphereCenter.x - pUAV->pos.x) / distanceToSphere, 
        (sphereCenter.y - pUAV->pos.y) / distanceToSphere,
        (sphereCenter.z - pUAV->pos.z) / distanceToSphere
    };

    /* loop until 60 seconds have passed and all teapots have met end condition */
    while (endAll < 15)
    {
        /* update position, velocity, acceleration with kinematic equations */
        if (magnitude3D(&pUAV->pos, &sphereCenter) > 10)
        {
            /* has not yet reached sphere */
            getNewAcc(pUAV, &directionToSphere);
            getNewVel(pUAV, true);
        }
        else
        {
            /* has reached sphere */
            getNewAcc(pUAV);
            getNewVel(pUAV);
        }
        getNewPos(pUAV);

        /* check for collisions */
        for (int i = 0; i < numTeapots; i++)
        {
            if (i != pUAV->index && magnitude3D(&pUAV->pos, &teapots[i].pos) <= 0.1)
            {
                /* lock mutex and swap velocity vectors */
                triple temp = pUAV->vel;
                pUAV->vel = teapots[i].vel;
                swap_mtx.lock();
                teapots[i].vel = pUAV->vel;
                swap_mtx.unlock();
            }
        }

        /* check for end condition */
        chrono::duration<double> elapsed = chrono::system_clock::now() - start;
        if (magnitude3D(&pUAV->pos, &sphereCenter) <= 10 && elapsed >= chrono::seconds(60) && !pUAV->end)
        {
            endAll++;
            pUAV->end = true;
        }

        /* thread sleeps for 10ms */
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

/* opengl display function  - draws field and sphere, updates teapot locations using object data */
void display () // FINISH
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    /* draw teapot as threads handle updating positional data */
    glLoadIdentity();
    // loadObj("teapot.obj", );

    /* poll threads and redraw teapots */
    for (int i = 0; i < numTeapots; i++)
    {

    }

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

    /* draw sphere */

    glLoadIdentity();
    glutSwapBuffers();
}

int main (int argc, char* argv[]) 
{
    /* seed random number generator */
    srand(time(NULL));

    /* set initial positional vectors */
    double currentY = -(fieldLength / 2);
    for (int i = 0; i < numTeapots; i++)
    {
        /* set index value */
        teapots[i].index = i;

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
    
    /* initialize opengl stuff */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("UAV Simulation");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glutDisplayFunc(display);

    /* set starting time */
    start = chrono::system_clock::now();

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

    /* begin main loop - polls every 30ms */
    while (true)
    {
        glutMainLoopEvent();
        this_thread::sleep_for(chrono::milliseconds(30));
    }
}