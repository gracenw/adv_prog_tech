#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <GL/glut.h>

using namespace std;

struct triple
{
    double x;
    double y;
    double z;
};

// void threadFunction(ECE_UAV * pUAV)
// {
//     // update kinematic info every 10 ms
// }

class ECE_UAV 
{
    public:
        /* member variables */
        double mass;
        triple pos;
        triple vel;
        triple acc;
        // thread thr;

        /* member functions */
        void start()
        {
            // threadFunction(this);
        }
};

void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5,-0.5,0.0);
		glVertex3f(0.5,0.0,0.0);
		glVertex3f(0.0,0.5,0.0);
	glEnd();

        glutSwapBuffers();
}

int main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("UAV Simulation");
    glutDisplayFunc(renderScene);
    glutMainLoop();

    /* return successfully */
    return 0;
}