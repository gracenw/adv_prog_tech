/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 11/21/21

  Description: 
  CUDA application to calculate the steady-temperature of a thin plate using
  GPU computation; prints execution time to console and final temperature
  values to CSV file.
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <cuda_runtime.h>

#define MAX_BOUNDARY 100
#define INIT_TEMP 20

/* 
  function: checkArgs
  ensures the command line argument fits requirements
*/
unsigned checkArgs(const char *arg)
{
    /* check that all chars in arg are numbers */
    for (int i = 0; i < strlen(arg); i++)
    {
        if (int(arg[i]) < 48 || int(arg[i]) > 57)
        {
            std::cout << "Invalid parameters, please check your values." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /* convert to unsigned long */
    unsigned val = strtoul(arg, NULL, 10);

    /* check that unsigned long is not zero or negative */
    if (val <= 0)
    {
        std::cout << "Invalid parameters, please check your values." << std::endl;
        exit(EXIT_FAILURE);
    }

    return val;
}

/* 
  function: calculateTemp
  calculates the temperature of the CUDA block using the average temperature
  of the four surrounding points on the thin plate.
*/
__global__ void calculateTemp(double *h, double *g, unsigned N)
{
    /* get index from block id since each point on plate is given a block */
    int i = blockIdx.x;

    /* calculate temperatures of interior points only */
    if (((i + 1) % N) != 0 && (i % N) != 0 && i > N && i < (N * (N - 1)))
    {
        g[i] = 0.25 * (h[i - 1] + h[i + 1] + h[i - N] + h[i + N]);
    }
}

/* 
  function: copyMatrix
  copies the new values stored in 'temporary' matrix G into the final matrix H.
*/
__global__ void copyMatrix(double *h, double *g, unsigned N)
{
    /* get index from block id since each point on plate is given a block */
    int i = blockIdx.x;

    /* copy current point value to matrix H */
    h[i] = g[i];
}

/* 
  function: main
  executes CUDA kernels (calculateTemp and copyMatrix) in loop for specified number
  of iterations to calculate final temperature matrix as well as execution time.
*/
int main(int argc, char *argv[])
{
    /* check arguments */
    unsigned N = 0;
    unsigned I = 0;

    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-N") == 0)
        {
            N = checkArgs(argv[i + 1]);
            N = N + 2;
        }
        else if (strcmp(argv[i], "-I") == 0)
        {
            I = checkArgs(argv[i + 1]);
        }
        else
        {
            std::cout << "Invalid parameters, please check your values." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /* make sure values were updated properly */
    if (N == 0 || I == 0)
    {
        std::cout << "Invalid parameters, please check your values." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* allocate cpu mem to array */
    size_t size = N * N * sizeof(double);
    double *H_h = (double *)malloc(size);
    double *H_g = (double *)malloc(size);

    /* initialize all array interior positions */
    for (int i = 0; i < (N * N); i++)
    {
        if (i > (0.3 * (N - 1)) && i < (0.7 * (N - 1)))
        {
            H_h[i] = MAX_BOUNDARY;
            H_g[i] = MAX_BOUNDARY;
        }
        else
        {
            H_h[i] = INIT_TEMP;
            H_g[i] = INIT_TEMP;
        }
    }

    /* allocate gpu mem to matrix H */
    double *D_h = NULL;
    cudaMalloc((void **)&D_h, size);

    /* allocate gpu mem to matrix G */
    double *D_g = NULL;
    cudaMalloc((void **)&D_g, size);

    /* send matrix H to gpu */
    cudaMemcpy(D_h, H_h, size, cudaMemcpyHostToDevice);

    /* send matrix G to gpu */
    cudaMemcpy(D_g, H_g, size, cudaMemcpyHostToDevice);

    /* initialize event timing variables */
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    /* get device properties to set num of threads */
    cudaDeviceProp properties;
    cudaGetDeviceProperties(&properties, 0);
    int threadsPerBlock = properties.maxThreadsPerBlock;

    /* execute kernel with in loop with timing */
    cudaEventRecord(start);
    while (I > 0)
    {
        calculateTemp<<<(N * N), threadsPerBlock>>>(D_h, D_g, N);
        cudaDeviceSynchronize();
        copyMatrix<<<(N * N), threadsPerBlock>>>(D_h, D_g, N);
        cudaDeviceSynchronize();
        I--;
    }
    cudaEventRecord(stop);

    /* synchronize event at end of kernel execution */
    cudaEventSynchronize(stop);

    /* calculate kernel execution time & write to console */
    float milli = 0;
    cudaEventElapsedTime(&milli, start, stop);
    std::cout << std::fixed << std::setprecision(2) << milli << std::endl;

    /* copy data back to cpu */
    cudaMemcpy(H_h, D_h, size, cudaMemcpyDeviceToHost);

    /* evaluate results & write to csv */
    std::ofstream ofs("finalTemperatures.csv", std::ofstream::trunc);
    for (int i = 0; i < (N * N); i++)
    {
        ofs << std::fixed << std::setprecision(6) << H_h[i];
        if (((i + 1) % N) == 0 && i != 0)
        {
            ofs << std::endl;
        }
        else
        {
            ofs << ", ";
        }
    }
    ofs.close();

    /* free device memory */
    cudaFree(D_h);
    cudaFree(D_g);

    /* free host memory */
    free(H_h);
    free(H_g);

    /* return successfully */
    exit(EXIT_SUCCESS);
}