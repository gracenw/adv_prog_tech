/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 09/14/2021

  Description: 

  Program simulating an ant moving through a grid, with a certain
  amount of moves determined by command line argument, outputting
  the final number of black squares to a text file.
*/

/* include standard c++ namespace and necessary libraries */
#include <iostream>
#include <fstream>
#include <math.h>
#include "float.h"
#include "helper.h"

using namespace std;

#define point pair<float, float>
#define uvec pair<float, float>

static int side_length = 20;
static float height = sqrt(1.5) * side_length;
static float length = sqrt(100 + pow(height, 2));

inline uvec sub(const uvec & left, const uvec & right) { return make_pair((left.first - right.first), (left.second - right.second)); }
inline float dot(const uvec & left, const uvec & right) { return ((left.first * right.first) + (left.second * right.second)); }
inline uvec mul(float n, const uvec & vec) { return make_pair((vec.first * n), (vec.second * n)); }

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
        intersect = make_pair(FLT_MAX, FLT_MAX);
        return false;
    }
    else
    {
        float x = (b2 * c1 - b1 * c2) / determinant;
        float y = (a1 * c2 - a2 * c1) / determinant;

        // cout << x << ", " << y << endl;

        if (x < 10 && x > -10) 
        {
            intersect = make_pair(x, y);
            return true;
        }
        else
        {
            intersect = make_pair(FLT_MAX, FLT_MAX);
            return false;
        }
    }
}

int main(int argc, char* argv[]) 
{
    /* certify the command argument exists and conforms to requirements */
    float initial_x;
    if (!conform(argc, argv[1], initial_x, -10, 10)) /* must be below 10 or greater than -\10 */
    {
        cout << "Please include correct argument!" << endl;
        return 1;
    }

    /* begin with zero reflections */
    int reflections = 0;
    bool cont = true;
    int side = 1;

    /* calculate first incidence angle */
    float incidence = atan(initial_x / height);

    /* useful points */
    point A = make_pair(-10, height);
    point B = make_pair(10, height);
    point C = make_pair(0, 0);

    /* useful vectors */
    uvec ab = make_pair(1, 0); /* side 1 */
    uvec ab_norm = make_pair(0, -1);

    uvec ca = make_pair((-10 / length), (height / length)); /* side 2 */
    uvec ca_norm = make_pair((height / length), (10 / length));

    uvec cb = make_pair((10 / length), (height / length)); /* side 3 */
    uvec cb_norm = make_pair((-height / length), (10 / length));

    /* point object to keep track of beam position */
    point curr = make_pair(initial_x, height);

    /* calculate unit vector of first collision */
    uvec i_vec;
    if (incidence > 0) /* beam veers right */
    {
        i_vec = make_pair(sin(incidence), -cos(incidence));
    }
    else if (incidence < 0) /* beam veers left */
    {
        i_vec = make_pair(sin(incidence), cos(incidence));
    }
    else /* beam immediately exits cell after reflecting off AB */
    {
        cont = false; 
        reflections ++;
    }

    float m, b;
    point inter, coord1, coord2;
    uvec norm, reflect;

    /* perform series of reflections until beam exits cell */
    while(cont)
    {
        /* find collision point on new side */
        m = i_vec.second / i_vec.first;
        // cout << i_vec.first << ", " << i_vec.second << endl;
        // cout << m << endl;
        b = curr.second - m * curr.first;
        coord1 = make_pair(30, (m * 30 + b));
        // cout << coord1.first << ", " << coord1.second << endl;
        coord2 = make_pair(-30, (m * -30 + b));
        // cout << coord2.first << ", " << coord2.second << endl;

        // cout << coord1.first << ", " << coord1.second << endl;
        // cout << coord2.first << ", " << coord2.second << endl;

        /* determine which side beam will hit */
        if (side != 1 && intersect(coord1, coord2, A, B, inter)) /* intersects with AB */
        {
            norm = ab_norm;
            // cout << "ab" << endl;
            side = 1;
        }
        else if (side != 3 && intersect(coord1, coord2, A, C, inter)) /* intersects with CA */
        {
            norm = ca_norm;
            // cout << "ca" << endl;
            side = 3;
        }
        else if (side != 2 && intersect(coord1, coord2, C, B, inter)) /* intersects with CB */
        {
            norm = cb_norm;
            // cout << "cb" << endl;
            side = 2;
        }
        else 
        {
            cout << "Error" << endl;
            // cout << reflections << endl;
            return 1;
        }

        /* calculate reflection vector -> R = I - 2N(I . N) */
        reflect = sub(i_vec, mul((2 * dot(i_vec, norm)), norm));

        /* save intersection point as current point */
        curr.first = inter.first;
        curr.second = inter.second;

        /* save reflection vector as next incidence vector */
        i_vec.first = reflect.first;
        i_vec.second = reflect.second;

        /* increment number of reflections */
        reflections ++;

        /* check if reached exit */
        if (curr.second < 0.01)
        {
            cont = false;
        }

        cout << curr.first << ", " << curr.second << endl;
        // cout << i_vec.first << ", " << i_vec.second << endl << endl;

        // sleep(1);
    } 

    /* write output to file */
    ofstream ofs ("output3.txt", ofstream::trunc);
    ofs << reflections;
    ofs.close();

    return 0;
}