/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 09/14/2021

  Description: 

  Simple application to save the prime factors of a number
  read in from the command line to a text file.
*/

/* include standard c++ namespace and necessary libraries */
#include <iostream>
#include <fstream>
#include <math.h>
#include <list>
#include <iterator>
#include "helper.h"
using namespace std;

/*
  function: GetPrimeFactors
  solves for the prime factors of ulInputNumber and returns true/false if input
  is/is not prime; also saves prime factors in strOutput
*/
bool GetPrimeFactors(const unsigned long ulInputNumber, string & strOutput) 
{
    /* create empty list to hold prime factors */
    list<int> factors = { };

    /* create editable input number */
    unsigned long wip = ulInputNumber;

    /* divide wip by 2 until it is odd */
    while(wip % 2 == 0) 
    {
        wip /= 2;
        /* add 2 to list of prime factors */
        factors.push_back(2);
    }

    /* divide wip by odd factors until square root is reached */
    for (int i = 3; i <= sqrt(wip); i += 2) 
    {
        while (wip % i == 0) 
        {
            wip /= i;
            /* add i to list of prime factors */
            factors.push_back(i);
        }
    }

    /* if wip is still greater than 2, it itself is a prime factor */
    if (wip > 2) 
    {
        factors.push_back(wip);
    }

    /* check if factors has more than 1 and it isn't just itself */
    if (factors.size() != 1 && factors.front() != ulInputNumber) 
    {
        /* remove duplicate factors */
        factors.unique();

        /* sort factors by smallest to largest */
        factors.sort();

        /* for each factor in list, add to string */
        for (int num : factors) 
        {
            strOutput += to_string(num);
            strOutput += ",";
        }

        /* remove last extra comma */
        strOutput.pop_back();
        
        return true;
    }
    
    return false;
}

/* 
  function: main
  entry point, reads in unsigned long and calls GetPrimeFactors
*/
int main(int argc, char* argv[]) 
{
    /* certify the command argument exists and conforms to requirements */
    unsigned long ulInputNumber;
    if (!conform(argc, argv[1], ulInputNumber)) 
    {
        cout << "Please include correct argument!" << endl;
        return 1;
    }

    /* call function to determine prime factors of input */
    string strOutput = "";
    bool out = GetPrimeFactors(ulInputNumber, strOutput);
    
    /* write output to file */
    ofstream ofs ("output1.txt", ofstream::trunc);
    if (out)
    {
        ofs << strOutput;
    }
    else 
    {
        ofs << "No prime factors";
    }
    ofs.close();

    return 0;
}