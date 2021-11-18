#include <stdio.h>
#include <cuda_runtime.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fstream>

#define MAX_BOUNDARY    100
#define INIT_TEMP       20

/* 
  function: checkArgs
  ensures the command line argument fits requirements, 
  saves unsigned in placeholder and aborts otherwise
*/
void checkArgs(const char * arg, unsigned & placeholder) 
{
    /* check that all chars in arg are numbers */
    for (int i = 0; i < strlen(arg); i++) 
    {
        if (int(arg[i]) < 48 || int(arg[i]) > 57) 
        {
            std::cout << "Invalid parameters, please check your values." << std::endl;
            abort();
        }
    }

    /* convert to unsigned long */
    placeholder = strtoul(arg, NULL, 10);

    /* check that unsigned long is not zero or negative */
    if (placeholder <= 0) 
    {
        std::cout << "Invalid parameters, please check your values." << std::endl;
        abort();
    }
}

/* 
  function: calculateTemp
  
*/
__global__ void calculateTemp(double * h, unsigned N, unsigned I)
{
    while (I > 0)
    {
        double * g = (double *) malloc(N * N * sizeof(double));

        for (int i = 1; i < (N - 1); i++)
        {
            for (int j = 1; j < (N - 1); j++)
            {
                *(g + (j * N) + i) = 0.25 * *(h + (j * N) + i - 1) + *(h + (j * N) + i + 1) + *(h + ((j - 1) * N) + i) + *(h + ((j + 1) * N) + i);
            }
        }

        for (int i = 1; i < (N - 1); i++)
        {
            for (int j = 1; j < (N - 1); j++)
            {
                *(h + (j * N) + i) = *(g + (j * N) + i);
            }
        }

        free(g);
        I--;
    }
}

/* function: main
   
*/
int main(int argc, char * argv[])
{
    /* check arguments */
    unsigned N;
    unsigned I;
    int c;
    while ((c = getopt(argc, argv, "NI:")) != -1)
    {
        switch (c)
        {
            case 'N':
                checkArgs(optarg, N);
                N = N + 2;
                break;
            case 'I':
                checkArgs(optarg, I);
                break;
            case '?':
                std::cout << "Invalid parameters, please check your values." << std::endl;
                return 1;
            default:
                break;
        }
    }

    /* allocate cpu mem to array */
    size_t size = N * N * sizeof(double);
    double * H_h = (double *) malloc(size);

    /* initialize array interior values */
    for (int i = 1; i < (N - 1); i++)
    {
        for (int j = 1; j < (N - 1); j++)
        {
            *(H_h + (j * N) + i) = INIT_TEMP;
        }
    }

    /* initialize array exterior values */
    for (int i = 0; i < N; i++)
    {
        if (i > (0.3 * (N - 1)) && i < (0.7 * (N - 1)))
        {
            *(H_h + i) = MAX_BOUNDARY;
        }
        else
        {
            *(H_h + i) = INIT_TEMP;
        }
        *(H_h + ((N - 1) * N) + i) = INIT_TEMP;
    }
    for (int j = 1; j < (N - 1); j++)
    {
        *(H_h + (j * N)) = INIT_TEMP;
        *(H_h + (j * N) + N - 1) = INIT_TEMP;
    }

    /* get device properties to set num of threads and blocks */
    cudaDeviceProp properties;
    cudaGetDeviceProperties(&properties, 0);
    int maxThreadsPerBlock = properties.maxThreadsPerBlock;

    /* allocate gpu mem to array */
    double * D_h = NULL;
    cudaMalloc((void **) &D_h, size);

    /* send cpu data to gpu */
    cudaMemcpy(D_h, H_h, size, cudaMemcpyHostToDevice);

    /* initialize event timing variables */
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    /* execute kernel with timing */
    cudaEventRecord(start);
    calculateTemp<<<1, maxThreadsPerBlock>>>(D_h, N, I);
    cudaEventRecord(stop);

    /* synchronize gpu & event at end of kernel execution */
    cudaDeviceSynchronize();
    cudaEventSynchronize(stop);

    /* calculate kernel execution time & write to console */
    float milli = 0;
    cudaEventElapsedTime(&milli, start, stop);
    std::cout << std::fixed << std::setprecision(2) << milli;

    /* copy data back to cpu */
    cudaMemcpy(H_h, D_h, size, cudaMemcpyDeviceToHost);

    /* evaluate results & write to csv */
    std::ofstream ofs ("finalTemperatures.csv", std::ofstream::trunc);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            ofs << std::fixed << std::setprecision(8) << *(H_h + (j * N) + i) << ',';
        }
        ofs << std::endl;
    }
    ofs.close();

    /* free allocated memory */
    cudaFree(D_h);
    free(H_h);
    
    /* return successfully */
    return 0;
}