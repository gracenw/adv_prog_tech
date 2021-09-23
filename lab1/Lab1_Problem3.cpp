/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 09/23/2021

  Description: 

  Program simulating an ant moving through a grid, with a certain
  amount of moves determined by command line argument, outputting
  the final number of black squares to a text file.
*/

/* include standard c++ namespace and necessary libraries */
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
#include "float.h"
using namespace std;

/* for readability purposes */
#define point pair<float, float>
#define vector pair<float, float>

/* 
  for debugging purposes
  ostream & operator<<(ostream & stream, const pair<float, float> & p) { return stream << "" << p.first << ", " << p.second << ""; }
*/

/* checks that command line argument conforms to criteria */
bool conform(const int argc, const char * argv1, float & placeholder, int lower, int upper);

/* important variables */
static int sideLength = 20;
static float height = 17.3205;
static float hypoLength = sqrt(100 + pow(height, 2));

/* vectors functions for calculating reflection vector */
inline vector sub(const vector & left, const vector & right) { return make_pair((left.first - right.first), (left.second - right.second)); }
inline float dot(const vector & left, const vector & right) { return ((left.first * right.first) + (left.second * right.second)); }
inline vector mul(float n, const vector & vec) { return make_pair((vec.first * n), (vec.second * n)); }

/* adapted from https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/ */
bool intersect(point A, point B, point C, point D, point & intersect)
{
    float a1 = B.second - A.second;
    float b1 = A.first - B.first;
    float c1 = a1 * (A.first) + b1 * (A.second);
  
    float a2 = D.second - C.second;
    float b2 = C.first - D.first;
    float c2 = a2 * (C.first) + b2 * (C.second);
  
    float determinant = a1 * b2 - a2 * b1;
  
    if (determinant == 0)
    {
        return false;
    }
    else
    {
        float x = (b2 * c1 - b1 * c2) / determinant;
        float y = (a1 * c2 - a2 * c1) / determinant;

        if (x < 10 && x > -10 && y > 0 && y <= height) 
        {
            intersect = make_pair(x, y);
            return true;
        }
        else
        {
            return false;
        }
    }
}

/* 
  function: main
  entry point, reads performs ray tracing of laser beam in white cell until beam
  exits cell, writes total number of reflections to text file
*/
int main(int argc, char* argv[]) 
{
    /* certify the command argument exists and conforms to requirements */
    float initialX;
    if (!conform(argc, argv[1], initialX, -(sideLength / 2), (sideLength / 2))) /* must be below 10 or greater than -10 */
    {
        cout << "Please include correct argument!" << endl;
        return 1;
    }

    /* begin with zero reflections, starting side AB */
    int reflections = 0;
    int currentSide = 1;

    /* calculate first incidence angle */
    float incidenceAngle = atan(initialX / height);

    /* useful points */
    point A = make_pair(-(sideLength / 2), height);
    point B = make_pair((sideLength / 2), height);
    point C = make_pair(0, 0);

    /* useful vectors */
    vector side1Norm = make_pair(0, -1); /* side AB */
    vector side2Norm = make_pair((height / hypoLength), ((sideLength / 2) / hypoLength)); /* side CA */
    vector side3Norm = make_pair(-(height / hypoLength), ((sideLength / 2) / hypoLength)); /* side CB */

    /* point object to keep track of beam position */
    point currentPoint = make_pair(initialX, height);

    /* calculate unit incidence vector of first collision */
    vector incidenceVector;
    if (incidenceAngle != 0) /* beam veers left or right */
    {
        incidenceVector = make_pair(sin(incidenceAngle), -cos(incidenceAngle));
    }
    else /* beam immediately exits cell after reflecting off AB */
    {
        currentPoint = make_pair(0, 0);
        reflections ++;
    }
    
    float m, b;
    point intersectPoint, coord1, coord2;
    vector normVector, reflectVector;

    /* perform series of reflections until beam exits cell */
    if (currentPoint.first != 0) 
    {
        do
        {
            /* find collision point on new side */
            m = incidenceVector.second / incidenceVector.first;
            b = currentPoint.second - m * currentPoint.first;
            coord1 = make_pair(20, (m * 20 + b));
            coord2 = make_pair(-20, (m * -20 + b));

            /* determine which side beam will hit */
            if (currentSide != 1 && intersect(coord1, coord2, A, B, intersectPoint)) /* intersects with AB */
            {
                normVector = side1Norm;
                currentSide = 1;
            }
            else if (currentSide != 3 && intersect(coord1, coord2, A, C, intersectPoint)) /* intersects with CA */
            {
                normVector = side2Norm;
                currentSide = 3;
            }
            else if (currentSide != 2 && intersect(coord1, coord2, C, B, intersectPoint)) /* intersects with CB */
            {
                normVector = side3Norm;
                currentSide = 2;
            }
            else 
            {
                cout << "Error - stopped after " << reflections << " reflections" << endl;
                return 1;
            }

            /* calculate reflection vector -> R = I - 2N(I . N) */
            reflectVector = sub(incidenceVector, mul((2 * dot(incidenceVector, normVector)), normVector));

            /* save intersection point as current point */
            currentPoint = intersectPoint;

            /* save reflection vector as next incidence vector */
            incidenceVector = reflectVector;

            /* increment number of reflections */
            reflections ++;
        } while (currentPoint.second > 0.01);
    }

    /* write output to file */
    ofstream ofs ("output3.txt", ofstream::trunc);
    ofs << reflections;
    ofs.close();

    return 0;
}

/* 
  function: conform
  ensures the command line argument conforms to requirements, 
  saves float in placeholder and returns false if fail.
  accepts argc, argv1, float reference, and range of value
*/
bool conform(const int argc, const char * argv1, float & placeholder, int lower, int upper) 
{
    /* make sure there is a command line arg */
    if (argc < 2) 
    {
        return false;
    }

    /* check that all chars in argv[1] are numbers */
    for (int i = 0; i < strlen(argv1); i++) 
    {
        if (int(argv1[i]) < 45 || int(argv1[i]) > 57 || int(argv1[1]) == 47)
        {
            return false;
        }
    }

    /* convert to long */
    placeholder = strtof(argv1, NULL);

    /* check that long is in correct range */
    if (placeholder > lower && placeholder < upper) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}