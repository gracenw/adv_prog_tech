/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 

  Description: 
  
*/

/* include standard c++ namespace and necessary libraries */
#include <iostream>
#include <fstream>
#include <thread>
#include <random>
#include <atomic>
#include <set>
#include <chrono>
#include <list>
#include <iomanip>
using namespace std;

/* total number of moves, shared between threads */
atomic_int totalMoves(0);

/* struct for holding ant's placement, it has a seed, and number of moves */
struct laboringAnt 
{
    int x, y, numMoves;
    bool hasSeed;
};

/* 
  function: laboriousAnt()
  
*/
void laboriousAnt(const unsigned numRuns)
{
    for (int n = 0; n < numRuns; n ++)
    {
        /* create an ant */
        struct laboringAnt ant = { 2, 2, 0, false };

        /* random number generation tools */
        default_random_engine generator;
        generator.seed(chrono::system_clock::now().time_since_epoch().count());
        uniform_int_distribution<int> twoMoves(0,1);
        uniform_int_distribution<int> threeMoves(0,2);
        uniform_int_distribution<int> fourMoves(0,3);

        /* records next move from random number generator in each iteration */
        int nextMove;

        /* column locations of seeds in bottom and top rows */
        set<int> botRowSeeds = { 0, 1, 2, 3, 4 };
        set<int> topRowSeeds = {};

        /* loop until all seeds have been moved to top row */
        while (topRowSeeds.size() < 5)
        {
            /* increment number of moves */ 
            ant.numMoves ++;

            /* sides & corners */
            if (ant.x == 0 || ant.x == 4 || ant.y == 0 || ant.y == 4)
            {
                /* drop seed in top row */
                if (ant.hasSeed && ant.y == 4 && topRowSeeds.find(ant.x) == topRowSeeds.end())
                {
                    topRowSeeds.insert(ant.x);
                    ant.hasSeed = false;
                }

                /* pick up seed in bottom row */
                if (!ant.hasSeed && ant.y == 0 && botRowSeeds.find(ant.x) != botRowSeeds.end())
                {
                    botRowSeeds.erase(ant.x);
                    ant.hasSeed = true;
                }

                /* check if in a corner */
                {
                    nextMove = twoMoves(generator);
                    
                    if (ant.x == 0 && ant.y == 4)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* down */
                                ant.y --;
                                break;
                            case 1: /* right */
                                ant.x ++;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                    if (ant.x == 4 && ant.y == 4)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* down */ 
                                ant.y --;
                                break;
                            case 1: /* left */
                                ant.x --;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                    if (ant.x == 0 && ant.y == 0)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* up */
                                ant.y ++;
                                break;
                            case 1: /* right */
                                ant.x ++;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                    if (ant.x == 4 && ant.y == 0)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* up */
                                ant.y ++;
                                break;
                            case 1: /* left */
                                ant.x --;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                }

                /* not in a corner but still on a side */
                {
                    nextMove = threeMoves(generator);

                    /* bottom row */
                    if (ant.y == 0)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* up */
                                ant.y ++;
                                break;
                            case 1: /* left */ 
                                ant.x --;
                                break;
                            case 2: /* right */
                                ant.x ++;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                    /* top row */
                    if (ant.y == 4)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* down */
                                ant.y --;
                                break;
                            case 1: /* left */
                                ant.x --;
                                break;
                            case 2: /* right */
                                ant.x ++;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                    /* far left column */
                    if (ant.x == 0)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* right */
                                ant.x ++;
                                break;
                            case 1: /* up */
                                ant.y ++;
                                break;
                            case 2: /* down */
                                ant.y --;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                    /* far right column */
                    if (ant.x == 4)
                    {
                        switch(nextMove) 
                        {
                            case 0: /* left */
                                ant.x --;
                                break;
                            case 1: /* up */
                                ant.y ++;
                                break;
                            case 2: /* down */
                                ant.y --;
                                break;
                            default:
                                break;
                        }

                        continue;
                    }
                }
            }
            /* middle */
            else 
            {
                nextMove = fourMoves(generator);

                switch(nextMove) 
                {
                    case 0: /* up */
                        ant.y ++;
                        break;
                    case 1: /* down */
                        ant.y --;
                        break;
                    case 2: /* left */
                        ant.x --;
                        break;
                    case 3: /* right */
                        ant.x ++;
                        break;
                    default:
                        break;
                }
            }
        }

        totalMoves += ant.numMoves;
    }
}

/* 
  function: main()
  creates thread for laboriousAnt function, write number of threads, expected
  steps, and number of runs needed for convergence to output file
*/
int main(int argc, char* argv[]) 
{
    /* determine number of threads system can support */
    const unsigned numThreads = thread::hardware_concurrency();

    /* number of runs per thread */
    const unsigned numRuns = strtoul(argv[1], NULL, 10);

    /* expected number of runs - used for looping until convergence is reached */
    double expectedMoves;

    /* total number of runs across all threads */
    const unsigned totalRuns = numRuns * numThreads;

    /* create array of threads and instantiate each thread */
    thread threads[numThreads];
    for (int i = 0; i < numThreads; i ++)
    {
        threads[i] = thread(laboriousAnt, numRuns);
    }

    /* join all threads */
    for (int i = 0; i < numThreads; i ++)
    {
        threads[i].join();
    }

    expectedMoves = (double) totalMoves / totalRuns;

    /* write output to file */
    ofstream ofs ("ProblemOne.txt", ofstream::trunc);
    ofs << "Number of threads created: " << numThreads << endl << endl;
    ofs << "Expected number of steps: " << setprecision(10) << expectedMoves << endl << endl;
    ofs << "Total number of runs needed for solution convergence: " << totalRuns << endl;
    ofs.close();

    /* return successfully */
    return 0;
}