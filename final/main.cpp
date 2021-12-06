#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <GL/glew.h>
// #include <GL/gl.h>
// #include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <shader.hpp>
#include <objloader.hpp>
#include <controls.hpp>

using namespace std;

GLFWwindow* window;

/* generally useful static global variables */
static const double fieldWidth = 48.80;
static const double fieldLength = 91.44;
static const unsigned numTeapots = 15;
static const unsigned k = 1;

/* threading variables */
mutex swap_mtx;
atomic_uint64_t endAll(0);

/* clock starting time point to compute time elapsed */
chrono::system_clock::time_point start_clock;

/* struct to easily access positional data */
struct triple
{
    double x;
    double y;
    double z;
} sphereCenter{ 0, 0, 50 }, origin{ 0, 0, 0 };

/* simple function to find the magnitude of a 3D vector */
inline double magnitude3D (triple * left, triple * right)
{
    return ( sqrt( pow((left->x - right->x), 2) + pow((left->y - right->y), 2) + pow((left->z - right->z), 2) ) );
}

/* class representating a teapot UAV */
class ECE_UAV 
{
    public:
        /* constructor */
        ECE_UAV() { };

        /* member variables */
        double mass = 1;
        bool end = false;
        int index;
        triple pos, vel, acc;
        thread thr;

        /* member functions */
        thread start ();
        void setNewPos ();
        void setNewVel (bool twoMSCondition);
        void setNewAcc (triple * directionalVector);
};

/* sets new positional data with UAV pointer */
void ECE_UAV::setNewPos ()
{
    chrono::duration<double> elapsed = chrono::system_clock::now() - start_clock;
    this->pos.x = this->pos.x + this->vel.x * elapsed.count() + 0.5 * this->acc.x * pow(elapsed.count(), 2);
    this->pos.y = this->pos.y + this->vel.y * elapsed.count() + 0.5 * this->acc.y * pow(elapsed.count(), 2);
    this->pos.z = this->pos.z + this->vel.z * elapsed.count() + 0.5 * this->acc.z * pow(elapsed.count(), 2);
}

/* sets new velocity values with UAV pointer */
void ECE_UAV::setNewVel (bool twoMSCondition = false)
{
    chrono::duration<double> elapsed = chrono::system_clock::now() - start_clock;
    this->vel.x = this->vel.x + this->acc.x * elapsed.count();
    this->vel.y = this->vel.y + this->acc.y * elapsed.count();
    this->vel.z = this->vel.z + this->acc.z * elapsed.count();

    if (twoMSCondition)
    {
        /* ensure velocity is not greater than 2m/s if teapot hasnt yet reached sphere */
        while (magnitude3D(&this->vel, &origin) > 2)
        {
            /* divide by two until magnitude is small enough */
            this->vel.x /= 1.5;
            this->vel.y /= 1.5;
            this->vel.z /= 1.5;
        }
    }
    else 
    {
        /* ensure that teapot maintains speed between 2m/s and 10m/s around sphere */
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
}

/* sets new acceleration values with UAV pointer */
void ECE_UAV::setNewAcc (triple * directionalVector = NULL)
{
    /* determine magnitude of gravity and sphere force */
    double gravityForceZ = -10;
    double forceMagnitude = k * (10 - (magnitude3D(&this->pos, &sphereCenter)));
    
    triple force;
    if (directionalVector == NULL)
    {
        /* calculate force of sphere vector using a random unit vector and magnitude */
        triple randomVector { (double) rand(), (double) rand(), (double) rand() };
        double randMagnitude = magnitude3D(&randomVector, &origin);
        randomVector.x /= randMagnitude; // FINISH 
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
        /* divide by two until magnitude is small enough */
        force.x /= 1.5;
        force.y /= 1.5;
        force.z /= 1.5;
    }

    /* assign new acceleration using f = ma, m = 1 */
    this->acc.x = force.x;
    this->acc.y = force.y;
    this->acc.z = force.z;
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
            (*pUAV).setNewAcc(&directionToSphere);
            (*pUAV).setNewVel(true);
        }
        else
        {
            /* has reached sphere */
            (*pUAV).setNewAcc();
            (*pUAV).setNewVel();
        }
        (*pUAV).setNewPos();

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
        chrono::duration<double> elapsed = chrono::system_clock::now() - start_clock;
        if (magnitude3D(&pUAV->pos, &sphereCenter) <= 10 && elapsed >= chrono::seconds(60) && !pUAV->end)
        {
            endAll++;
            pUAV->end = true;
        }

        /* thread sleeps for 10ms */
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

thread ECE_UAV::start ()
{
    return thread(threadFunction, this);
}

int main (int argc, char* argv[]) 
{
    // /* seed random number generator */
    // srand(time(NULL));

    // /* set initial positional vectors */
    // double currentY = -(fieldLength / 2);
    // for (int i = 0; i < numTeapots; i++)
    // {
    //     /* set index value */
    //     teapots[i].index = i;

    //     /* x positions */
    //     if (i < 5)
    //     {
    //         teapots[i].pos.x = fieldWidth / 2;
    //     }
    //     else if (i < 10)
    //     {
    //         teapots[i].pos.x = 0;
    //     }
    //     else 
    //     {
    //         teapots[i].pos.x = -(fieldWidth / 2);
    //     }

    //     /* y positions */
    //     teapots[i].pos.y = currentY;
    //     if (currentY == (fieldLength / 2))
    //     {
    //         currentY = -(fieldLength / 2);
    //     }
    //     else
    //     {
    //         currentY += (fieldLength / 4);
    //     }
        
    //     /* z positions */
    //     teapots[i].pos.z = 0;
    // }

    // /* set starting time */
    // start_clock = chrono::system_clock::now();

    // /* launch threads with threadFunction */
    // for (int i = 0; i < numTeapots; i ++)
    // {
    //     teapots[i].thr = teapots[i].start();
    // }

    // /* join all threads */
    // for (int i = 0; i < numTeapots; i ++)
    // {
    //     // teapots[i].thr.join();
    // }
    
    /* initialize opengl stuff */
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}