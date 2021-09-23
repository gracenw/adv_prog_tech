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
#include "float.h"
#include "Lab1_Helper.h"

using namespace std;

#define point pair<float, float>
#define uvec pair<float, float>

ostream & operator<<(ostream & stream, const pair<float, float> & p) { return stream << "" << p.first << ", " << p.second << ""; }

static int side_length = 20;
static float height = 17.3205;
static float hypo_length = sqrt(100 + pow(height, 2));

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

int main(int argc, char* argv[]) 
{
    /* certify the command argument exists and conforms to requirements */
    float initial_x;
    if (!conform(argc, argv[1], initial_x, -(side_length / 2), (side_length / 2))) /* must be below 10 or greater than -10 */
    {
        cout << "Please include correct argument!" << endl;
        return 1;
    }

    /* begin with one reflection off AB */
    int reflections = 0;
    int side = 1;

    /* calculate first incidence angle */
    float incidence = atan(initial_x / height);

    /* useful points */
    point A = make_pair(-(side_length / 2), height);
    point B = make_pair((side_length / 2), height);
    point C = make_pair(0, 0);

    /* useful vectors */
    uvec ab = make_pair(1, 0); /* side 1 */
    uvec ab_norm = make_pair(0, -1);

    uvec ca = make_pair(-((side_length / 2) / hypo_length), (height / hypo_length)); /* side 2 */
    uvec ca_norm = make_pair((height / hypo_length), ((side_length / 2) / hypo_length));

    uvec cb = make_pair(((side_length / 2) / hypo_length), (height / hypo_length)); /* side 3 */
    uvec cb_norm = make_pair(-(height / hypo_length), ((side_length / 2) / hypo_length));

    /* point object to keep track of beam position */
    point curr = make_pair(initial_x, height);

    /* calculate unit vector of first collision */
    uvec i_vec;
    if (incidence != 0) /* beam veers left or right */
    {
        i_vec = make_pair(sin(incidence), -cos(incidence));
    }
    else /* beam immediately exits cell after reflecting off AB */
    {
        curr = make_pair(0, 0);
        reflections ++;
    }
    
    float m, b;
    point inter, coord1, coord2;
    uvec norm, reflect;

    /* perform series of reflections until beam exits cell */
    if (curr.first != 0) 
    {
        do
        {
            /* find collision point on new side */
            m = i_vec.second / i_vec.first;
            b = curr.second - m * curr.first;
            coord1 = make_pair(20, (m * 20 + b));
            coord2 = make_pair(-20, (m * -20 + b));

            /* determine which side beam will hit */
            if (side != 1 && intersect(coord1, coord2, A, B, inter)) /* intersects with AB */
            {
                norm = ab_norm;
                side = 1;
            }
            else if (side != 3 && intersect(coord1, coord2, A, C, inter)) /* intersects with CA */
            {
                norm = ca_norm;
                side = 3;
            }
            else if (side != 2 && intersect(coord1, coord2, C, B, inter)) /* intersects with CB */
            {
                norm = cb_norm;
                side = 2;
            }
            else 
            {
                cout << "Error - stopped after " << reflections << " reflections" << endl;
                return 1;
            }

            /* calculate reflection vector -> R = I - 2N(I . N) */
            reflect = sub(i_vec, mul((2 * dot(i_vec, norm)), norm));
            
            // cout << "intersection: " << inter << endl;
            // cout << "normal: " << norm << endl;
            // cout << "incidence: " << i_vec << endl;
            // cout << "reflection: " << reflect << endl;
            // cout << "reflections: " << reflections << endl << "--------" << endl;

            /* save intersection point as current point */
            curr = inter;

            /* save reflection vector as next incidence vector */
            i_vec = reflect;

            /* increment number of reflections */
            reflections ++;
        } while (curr.second > 0.01);
    }

    /* write output to file */
    ofstream ofs ("output3.txt", ofstream::trunc);
    ofs << reflections;
    // cout << "Successfully exited after " << reflections << " reflections" << endl;
    ofs.close();

    return 0;
}