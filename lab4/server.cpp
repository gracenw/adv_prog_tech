/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 

  Description: 
  
*/

#include <SFML/Network.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char* argv[])
{
    /* make sure there are correct number of args*/
    if (argc != 2) 
    {
        cout << "Invalid command line argument detected: incorrect number of arguments passed" << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* check that all chars in argv[2] are numbers */
    for (int i = 0; i < strlen(argv[1]); i++) 
    {
        if (int(argv[1][i]) < 48 || int(argv[1][i]) > 57) 
        {
            cout << "Invalid command line argument detected: non-numeric values in port number" << endl;
            cout << "Please check your values and press any key to end the program!" << endl;
            cin.get();
            return EXIT_FAILURE;
        }
    }

    /* convert to unsigned long */
    unsigned long port_number = strtoul(argv[1], NULL, 10);

    /* check that port number is in correct range */
    if (port_number < 61000 || port_number > 65535) 
    {
        cout << "Invalid command line argument detected: port number outside accepted range" << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }


    /* return successfully */
    return EXIT_SUCCESS;
}