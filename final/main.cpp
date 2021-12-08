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
static const unsigned k = 1;

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
    triple pos, vel, acc;
    thread thr;

    /* member functions */
    thread start();
    void setNewPos();
    void setNewVel();
    void setNewAcc();
};

/* sets new positional data with UAV pointer */
void ECE_UAV::setNewPos()
{
    chrono::duration<double> elapsed = chrono::system_clock::now() - start_clock;
    this->pos.x = this->pos.x + this->vel.x * (elapsed.count() - 5) + 0.5 * this->acc.x * pow((elapsed.count() - 5), 2);
    this->pos.y = this->pos.y + this->vel.y * (elapsed.count() - 5) + 0.5 * this->acc.y * pow((elapsed.count() - 5), 2);
    this->pos.z = this->pos.z + this->vel.z * (elapsed.count() - 5) + 0.5 * this->acc.z * pow((elapsed.count() - 5), 2);
}

/* sets new velocity values with UAV pointer */
void ECE_UAV::setNewVel()
{
    chrono::duration<double> elapsed = chrono::system_clock::now() - start_clock;
    // cout << "time: " <<  (elapsed.count() - 5) << endl;
    this->vel.x = this->vel.x + this->acc.x * (elapsed.count() - 5);
    this->vel.y = this->vel.y + this->acc.y * (elapsed.count() - 5);
    this->vel.z = this->vel.z + this->acc.z * (elapsed.count() - 5);

    /* ensure that uav maintains speed between 2m/s and 10m/s around sphere */
    while (magnitude3D(&this->vel, &origin) > 10)
    {
        /* divide by two until magnitude is small enough */
        this->vel.x /= 1.5;
        this->vel.y /= 1.5;
        this->vel.z /= 1.5;
    }
    while (magnitude3D(&this->vel, &origin) < 2)
    {
        /* divide by two until magnitude is small enough */
        this->vel.x *= 1.5;
        this->vel.y *= 1.5;
        this->vel.z *= 1.5;
    }
}

/* sets new acceleration values with UAV pointer */
void ECE_UAV::setNewAcc()
{
    triple force;

    /* determine magnitude of gravity and sphere force */
    double gravityForceZ = -10;
    double forceMagnitude = k * (10 - (magnitude3D(&this->pos, &sphereCenter)));

    /* calculate force of sphere vector using a random unit vector and magnitude */
    triple randomVector{(sphereCenter.y - this->pos.y), -(sphereCenter.x - this->pos.x), (double)(rand() % 10)};
    double randMagnitude = magnitude3D(&randomVector, &origin);
    randomVector.x /= randMagnitude;
    randomVector.y /= randMagnitude;
    randomVector.z /= randMagnitude;

    /* calculate final total force of sphere (modeled as spring) and gravity */
    force.x = randomVector.x * forceMagnitude;
    force.y = randomVector.y * forceMagnitude;
    force.z = randomVector.z * forceMagnitude + gravityForceZ;

    /* check that magnitude of force is less than 20 */
    while (magnitude3D(&force, &origin) > 20)
    {
        /* divide by 1.5 until magnitude is small enough */
        force.x /= 1.5;
        force.y /= 1.5;
        force.z /= 1.5;
    }

    /* assign new acceleration using f = ma, m = 1 */
    this->acc.x = force.x;
    this->acc.y = force.y;
    this->acc.z = force.z;
}

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

    /* loop until 60 seconds have passed and all uavs have met end condition */
    while (endAll < 15)
    {
        /* update position, velocity, acceleration with kinematic equations */
        if (magnitude3D(&pUAV->pos, &sphereCenter) > 10 && !firstContact)
        {
            /* has not yet reached sphere - not using functions to update position */
            double scale = 0.0205;

            /* scale was calculated so uavs do not move more than 2m/s */
            pUAV->pos.x += (scale * directionToSphere.x);
            pUAV->pos.y += (scale * directionToSphere.y);
            pUAV->pos.z += (scale * directionToSphere.z);
        }
        else
        {
            /* has reached sphere */
            firstContact = true;
            (*pUAV).setNewAcc();
            (*pUAV).setNewVel();
            (*pUAV).setNewPos();
            
            // cout << "position: " << pUAV->pos << endl;
            // cout << "velocity: " << pUAV->vel << endl;
            // cout << "acceleration: " << pUAV->acc << endl << endl;
        }

        /* check for collisions */
        for (int i = 0; i < numUAVs; i++)
        {
            if (i != pUAV->index && magnitude3D(&pUAV->pos, &uavs[i].pos) <= 0.1)
            {
                /* lock mutex and swap velocity vectors */
                triple temp = pUAV->vel;
                pUAV->vel = uavs[i].vel;
                swap_mtx.lock();
                uavs[i].vel = pUAV->vel;
                swap_mtx.unlock();
            }
        }

        /* check for end condition */
        chrono::duration<double> elapsed = chrono::system_clock::now() - start_clock;
        if (magnitude3D(&pUAV->pos, &sphereCenter) <= 10 && elapsed >= chrono::seconds(60) && !pUAV->end)
        {
            endAll++;
            pUAV->end = true;
        }

        /* thread sleeps for 10ms */
        this_thread::sleep_for(chrono::milliseconds(10));
        // this_thread::sleep_for(chrono::seconds(5));
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
    }

    /* set starting time */
    start_clock = chrono::system_clock::now();

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

    /* convert between coordinate fields and draw each uav */
    for (int i = 0; i < numUAVs; i++)
    {
        // /* first attribute buffer: vertices */
		// glEnableVertexAttribArray(0);
		// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// /* second attribute buffer: uvs */
		// glEnableVertexAttribArray(1);
		// glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        /* convert between coordinate systems and load teapot */
        glPushMatrix();
        double glX = (((uavs[i].pos.y - -(fieldLength / 2)) * (-2.4 - 2.4)) / ((fieldLength / 2) - -(fieldLength / 2))) + 2.4;
        double glY = (((uavs[i].pos.z - 0) * (0.5 - -1)) / (60 - 0)) + -1;
        double glZ = (((-uavs[i].pos.x - -(fieldWidth / 2)) * (2.4 - 1.55)) / ((fieldWidth / 2) - -(fieldWidth / 2))) + 1.55;
        glTranslated(glX, glY, glZ);
        glColor3d(1.0, 0.0, 0.0);
        glutSolidTorus(0.02, 0.04, 10, 20);
        // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
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

    /* set perspective of camera */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2, 2, -1.5, 1.5, 1, 10);

    /* move camera in scene */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0, 1, -3);
    glRotated(30, 1, 0, 0);

    // /* load object */
	// vector<glm::vec3> normals;
	// vector<glm::vec2> uvs;
	// bool res = loadOBJ("teapot.obj", vertices, uvs, normals);

    // /* load into VBO */
	// glGenBuffers(1, &vertexbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// glGenBuffers(1, &uvbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	// glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    /* launch animation */
    launchAnimation();
}