/*
    Author: Gracen Wallace
    Class: ECE 6122 A
    Last Date Modified: 12/?/2021
    Description:
    3D Animation of flying torus UAVs around a sphere
    suspended above a football field

    Compiles and runs on pace-ice with:
    source setup.sh && cmake . && make && ./final
*/

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <cmath>
#include <ctime>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>

/* using GLUT */
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <glm.hpp>

using namespace std;

/* generally useful static global variables */
static const double fieldWidth = 48.80;
static const double fieldLength = 91.44;
static const unsigned numUAVs = 15;
static const unsigned k = 0.5;

/* useful object loading stuff */
unsigned int vertexbuffer, uvbuffer;
vector<glm::vec3> vertices;

/* threading variables */
mutex swap_mtx;
atomic_uint64_t endAll(0);
void runOpenGL();

/* clock starting time point to compute time elapsed */
chrono::system_clock::time_point start_clock;

/* struct to easily access positional data */
struct triple
{
    double x;
    double y;
    double z;
} sphereCenter{0, 0, 50}, origin{0, 0, 0};

/* print function for debugging purposes */
ostream& operator<<(ostream& os, const triple& t)
{
    os << "(" << t.x << ", " << t.y << ", " << t.z << ")";
    return os;
}

/* simple function to find the magnitude of a 3D vector */
inline double magnitude3D(triple *left, triple *right)
{
    return (sqrt(pow((left->x - right->x), 2) + pow((left->y - right->y), 2) + pow((left->z - right->z), 2)));
}
inline double magnitude3D(double x, double y, double z)
{
    return (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
}

/* class representating a UAV */
class ECE_UAV
{
public:
    /* constructor */
    ECE_UAV(){};

    /* member variables */
    double mass = 1;
    bool end = false;
    int index;
    triple pos;
    double theta, phi, rho;
    thread thr;

    /* member functions */
    thread start();
};

/* initialize uav objects here so collision conditions can be checked - will be declared in main */
ECE_UAV uavs[numUAVs];

/* function called by each UAV thread to update location every 10ms */
void threadFunction(ECE_UAV *pUAV)
{
    /* wait 5 seconds at beginning of simulation */
    this_thread::sleep_for(chrono::seconds(5));

    /* create initial unit vector for movement towards sphere center */
    double distanceToSphere = magnitude3D(&pUAV->pos, &sphereCenter);
    triple directionToSphere{
        (sphereCenter.x - pUAV->pos.x) / (distanceToSphere),
        (sphereCenter.y - pUAV->pos.y) / (distanceToSphere),
        (sphereCenter.z - pUAV->pos.z) / (distanceToSphere)};

    bool firstContact = false;

    double t = 0.0;

    /* loop until 60 seconds have passed and all uavs have met end condition */
    while (endAll < 15)
    {
        /* update position, velocity, acceleration with kinematic equations */
        if (magnitude3D(&pUAV->pos, &sphereCenter) > 10 && !firstContact)
        {
            /* 
                has not yet reached sphere
                using x = x0 + t * v + a * t^2 = x0 + v
                where a = 0 (constant velocity) and v is manually set
                scale was calculated to keep velocity at max 2m/s
            */
            double scale = 0.06;
            pUAV->pos.x += (scale * directionToSphere.x);
            pUAV->pos.y += (scale * directionToSphere.y);
            pUAV->pos.z += (scale * directionToSphere.z);
        }
        else
        {
            if (!firstContact)
            {
                /* set starting time */
                start_clock = chrono::system_clock::now();
            }

            /* has reached sphere */
            firstContact = true;
            
            pUAV->phi = acos(pUAV->pos.z / (magnitude3D(pUAV->pos.x, pUAV->pos.y, pUAV->pos.z)));
            pUAV->theta = atan(pUAV->pos.y / pUAV->pos.x);

            double deltaPhi = 0.01 * (double) rand()/RAND_MAX;
            double deltaTheta = 0.01 * (double) rand()/RAND_MAX;

            pUAV->pos.x = pUAV->rho * sin(pUAV->phi + deltaPhi) * cos(pUAV->theta + deltaTheta);
            // cout << pUAV->phi << endl << pUAV->theta << endl;
            pUAV->pos.y = pUAV->rho * sin(pUAV->phi + deltaPhi) * sin(pUAV->theta + deltaTheta);
            pUAV->pos.z = pUAV->rho * cos(pUAV->theta + deltaTheta) + 40;
        }

        /* check for collisions */
        for (int i = 0; i < numUAVs; i++)
        {
            if (i != pUAV->index && magnitude3D(&pUAV->pos, &uavs[i].pos) <= 0.1)
            {
                /* lock mutex and swap theta and phi */
                double temp_phi = pUAV->phi;
                double temp_theta = pUAV->theta;
                pUAV->phi = uavs[i].phi;
                pUAV->theta = uavs[i].theta;
                swap_mtx.lock();
                uavs[i].phi = temp_phi;
                uavs[i].theta = temp_theta;
                swap_mtx.unlock();
            }
        }

        /* check for end condition */
        chrono::duration<double> elapsed = (chrono::system_clock::now() - start_clock);
        if (magnitude3D(&pUAV->pos, &sphereCenter) <= 10 && elapsed >= chrono::seconds(60) && !pUAV->end)
        {
            endAll++;
            pUAV->end = true;
        }

        /* thread sleeps for 10ms */
        t += 0.1;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

/* function called by uav objects to start thread */
thread ECE_UAV::start()
{
    return thread(threadFunction, this);
}

void launchAnimation()
{
    /* seed random number generator */
    srand(time(NULL));

    /* set initial positional vectors */
    double currentY = -(fieldLength / 2);
    for (int i = 0; i < numUAVs; i++)
    {
        /* set index value */
        uavs[i].index = i;

        /* x positions */
        if (i < 5)
        {
            uavs[i].pos.x = fieldWidth / 2;
        }
        else if (i < 10)
        {
            uavs[i].pos.x = 0;
        }
        else
        {
            uavs[i].pos.x = -(fieldWidth / 2);
        }

        /* y positions */
        uavs[i].pos.y = currentY;
        if (currentY == (fieldLength / 2))
        {
            currentY = -(fieldLength / 2);
        }
        else
        {
            currentY += (fieldLength / 4);
        }

        /* z positions */
        uavs[i].pos.z = 0;

        uavs[i].rho = 10;
    }

    /* create main thread for opengl stuff */
    thread gl_thr = thread(runOpenGL);

    /* launch threads with threadFunction */
    for (int i = 0; i < numUAVs; i++)
    {
        uavs[i].thr = uavs[i].start();
    }

    /* join main gl thread */
    gl_thr.join();

    /* join all threads */
    for (int i = 0; i < numUAVs; i++)
    {
        uavs[i].thr.join();
    }
}

/* keep track of opacity */
double alpha = 0.5;
bool increasing = true;

/* display function called by main loop, updates scene*/
void glDisplay()
{
    /* clear scene */
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw green field */
    glBegin(GL_QUADS);
    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(-2.5, -1.0, 2.5);
    glVertex3d(2.5, -1.0, 2.5);
    glVertex3d(2.5, -1.0, 1.5);
    glVertex3d(-2.5, -1.0, 1.5);
    glEnd();

    /* alter opacity */ //FINISH
    if (alpha <= 1.0 && increasing)
    {
        if (alpha == 1.0)
        {
            increasing = false;
        }
        alpha += 0.01;
    }
    else if (alpha >= 0.5 && !increasing)
    {
        if (alpha == 0.5)
        {
            increasing = true;
        }
        alpha -= 0.01;
    }
    // cout << alpha << endl;

    /* convert between coordinate fields and draw each uav */
    for (int i = 0; i < numUAVs; i++)
    {        
        /* convert between coordinate systems and load object */
        glPushMatrix();
        double glX = (((uavs[i].pos.y - -(fieldLength / 2)) * (-2.4 - 2.4)) / ((fieldLength / 2) - -(fieldLength / 2))) + 2.4;
        double glY = (((uavs[i].pos.z - 0) * (0.5 - -1)) / (60 - 0)) + -1;
        double glZ = (((-uavs[i].pos.x - -(fieldWidth / 2)) * (2.4 - 1.55)) / ((fieldWidth / 2) - -(fieldWidth / 2))) + 1.55;
        glTranslated(glX, glY, glZ);
        glColor4d(1.0, 0.0, 0.0, alpha);
        glutSolidTorus(0.01, 0.02, 10, 20);
        glPopMatrix();
    }

    /* force redraw */
    glFlush();
}

/* function run by opengl dedicated thread to update scene */
void runOpenGL()
{
    while (true)
    {
        glutMainLoopEvent();
        glDisplay();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

/* main function initialiazing opengl window and launch~Cing animation */
int main(int argc, char **argv)
{
    /* initialize and create 400x400 window */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(1000, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("UAV Simulation");

    /* set display function and initialize camera within window */
    glutDisplayFunc(glDisplay);

    /* set background to black */
    glClearColor(0.0, 0.0, 0.0, 1.0);

    /* set blend function */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    /* set perspective of camera */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2, 2, -1.5, 1.5, 1, 10);

    /* move camera in scene */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0, 1, -3);
    glRotated(30, 1, 0, 0);

    /* launch animation */
    launchAnimation();
}