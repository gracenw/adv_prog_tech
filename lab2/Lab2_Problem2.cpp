/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 10/8/21

  Description: 
  Computes the integral of f(x) using the midpoint rule, given
  a certain number of subdivisions read in from the command. Writes
  final integral value to text file.
*/

/* include standard c++ namespace and necessary libraries */
#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <omp.h>
#include <iomanip>
using namespace std;

/* checks that command line argument conforms to criteria */
bool conform (const int argc, const char * argv1, unsigned long & placeholder);

/* 
  function: main
  calculates value of an integral using the midpoint rule, based on number of 
  subdivisions read in from command line
*/
int main (int argc, char* argv[]) 
{
    /* certify the command argument exists and conforms to requirements */
    unsigned long N;
    if (!conform(argc, argv[1], N)) 
    {
        cout << "Please include correct argument!" << endl;
        return 1;
    }

    /* variables for integral calculation */
    double estimation = 0;
    double integralMin = 0;
    double integralMax = log(2) / 7;
    double deltaX = (integralMax - integralMin) / N;

    #pragma omp parallel
    {
        #pragma omp for nowait
        for (int i = 0; i < N; i ++)
        {
            /* calculate area of current section using F(midpoint) and deltaX */
            double midpoint = deltaX * i + (deltaX / 2);
            double area = (14 * exp(7 * midpoint)) * deltaX;
            #pragma omp atomic
            estimation += (area);
        }
    }

    /* write output to file */
    ofstream ofs ("Lab2Prob2.txt", ofstream::trunc);
    ofs << fixed << setprecision(6) << estimation;
    ofs.close();

    /* return successfully */
    return 0;
}

/* 
  function: conform
  ensures the command line argument conforms to requirements, 
  saves unsigned long in placeholder and returns false if fail.
  accepts argc, argv1, and unsigned long reference
*/
bool conform (const int argc, const char * argv1, unsigned long & placeholder) 
{
    /* make sure there is a command line arg */
    if (argc < 2) 
    {
        return false;
    }

    /* check that all chars in argv[1] are numbers */
    for (int i = 0; i < strlen(argv1); i++) 
    {
        if (int(argv1[i]) < 48 || int(argv1[i]) > 57) 
        {
            return false;
        }
    }

    /* convert to unsigned long */
    placeholder = strtoul(argv1, NULL, 10);

    /* check that unsigned long is not zero or negative */
    if (placeholder <= 0) 
    {
        return false;
    }

    return true;
}