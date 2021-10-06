/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 10/6/21

  Description: 
  Random ant simulator where an ant moves seeds from the bottom row of a grid to 
  the top; creates threads and runs to convergence of final number of expected moves.
  Outputs number of threads, number of runs, and final expected moves to a text file.
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
atomic_uint64_t totalMoves(0);

/* struct for holding ant's placement, it has a seed, and number of moves */
struct laboringAnt 
{
    int x, y, numMoves;
    bool hasSeed;
};

/* 
  function: laboriousAnt()
  simulates random movement of ant around a grid until all seeds along bottom row
  have been moved to the top row
*/
void laboriousAnt (const unsigned numRuns)
{
    for (int n = 0; n < numRuns; n ++)
    {
        /* create an ant */
        struct laboringAnt ant = { 2, 2, -1, false };

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

            /* move based on current location */
            if (ant.y == 0)
            {
                /* pick up seed in bottom row */
                if (!ant.hasSeed && botRowSeeds.find(ant.x) != botRowSeeds.end())
                {
                    botRowSeeds.erase(ant.x);
                    ant.hasSeed = true;
                }

                if (ant.x == 0) 
                {
                    nextMove = twoMoves(generator);
                    switch(nextMove) /* bottom left corner */
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
                }
                else if (ant.x == 4) 
                {
                    nextMove = twoMoves(generator);
                    switch(nextMove) /* bottom right corner */
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
                }
                else
                {
                    nextMove = threeMoves(generator);
                    switch(nextMove) /* bottom mid row */
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
                }
            }
            else if (ant.y == 4)
            {
                /* drop seed in top row */
                if (ant.hasSeed && ant.y == 4 && topRowSeeds.find(ant.x) == topRowSeeds.end())
                {
                    topRowSeeds.insert(ant.x);
                    ant.hasSeed = false;
                }

                if (ant.x == 0)
                {
                    nextMove = twoMoves(generator);
                    switch(nextMove) /* top left corner */
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
                }
                else if (ant.x == 4)
                {
                    nextMove = twoMoves(generator);
                    switch(nextMove)  /* top right corner */
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
                }
                else
                {
                    nextMove = threeMoves(generator);
                    switch(nextMove) /* top mid row */
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
                }
            }
            else 
            {
                if (ant.x == 0)
                {
                    nextMove = threeMoves(generator);
                    switch(nextMove) /* left mid column */
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
                }
                else if (ant.x == 4)
                {
                    nextMove = threeMoves(generator);
                    switch(nextMove) /* right mid column */
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
                }
                else 
                {
                    nextMove = fourMoves(generator);
                    switch(nextMove) /* middle grid */
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
    /* measure execution time - start */
    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    /* determine number of threads system can support */
    const unsigned numThreads = thread::hardware_concurrency() - 1;

    /* number of runs per thread */
    unsigned numRuns = ceil((double) 10000000 / numThreads);

    /* total number of runs across all threads */
    unsigned totalRuns;

    /* expected number of runs */
    double expectedMoves;

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

    totalRuns = (numRuns * numThreads);

    /* calculate expected moves */
    expectedMoves = ((double) totalMoves) / ((double) totalRuns);

    /* measure execution time - end */
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    /* print time elapsed */
    const int64_t elapsed = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "Time elapsed: " << elapsed << " seconds" << endl;

    /* write output to file */
    ofstream ofs ("ProblemOne.txt", ofstream::trunc);
    ofs << "Number of threads created: " << (numThreads + 1) << endl << endl;
    ofs << "Expected number of steps: " << setprecision(9) << expectedMoves << endl << endl;
    ofs << "Total number of runs needed for solution convergence: " << totalRuns << endl;
    ofs.close();

    /* return successfully */
    return 0;
}