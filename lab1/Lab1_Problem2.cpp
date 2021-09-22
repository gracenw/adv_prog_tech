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
#include <set>
#include <iterator>
#include "Lab1_Helper.h"
using namespace std;

/* 
  function: main
  entry point, reads in unsigned long and moves ant around map, writes
  final count of black squares to text file
*/
int main(int argc, char* argv[]) 
{
    /* certify the command argument exists and conforms to requirements */
    unsigned long num_moves;
    if (!conform(argc, argv[1], num_moves)) 
    {
        cout << "Please include correct argument!" << endl;
        return 1;
    }

    /* create list of pairs containing coordinates of black squares */
    set<pair<int, int>> black;

    /* initialize placement on map */
    pair<int, int> curr = make_pair(0, 0);

    /* 0 = up, 1 = right, 2 = down, 3 = left */
    int dir = 0;

    while (num_moves > 0) 
    {
        /* determine whether current location is black or white */
        set<pair<int, int>>::iterator it = black.find(curr);

        if (it != black.end()) /* square is black */
        {
            /* flip square to white */
            black.erase(it);

            /* rotate 90 degrees counterclockwise and move forward one square */
            switch(dir) 
            {
                case 0: /* up */
                    dir = 3;
                    curr.first --;
                    break;
                case 1: /* right */
                    dir --;
                    curr.second ++;
                    break;
                case 2: /* down */
                    dir --;
                    curr.first ++;
                    break;
                case 3: /*left */
                    dir --;
                    curr.second --;
                    break;
                default:
                    break;
            }
        }
        else /* square is white */
        {
            /* flip square to black */
            black.insert(curr);

            /* rotate 90 degrees clockwise and move forward one square */
            switch(dir) 
            {
                case 0: /* up */
                    dir ++;
                    curr.first ++;
                    break;
                case 1: /* right */
                    dir ++;
                    curr.second --;
                    break;
                case 2: /* down */
                    dir ++;
                    curr.first --;
                    break;
                case 3: /*left */
                    dir = 0;
                    curr.second ++;
                    break;
                default:
                    break;
            }
        }

        /* decrement number of moves */
        num_moves --;
    }

    /* write output to file */
    ofstream ofs ("output2.txt", ofstream::trunc);
    ofs << black.size();
    ofs.close();

    return 0;
}