#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <cuda_runtime.h>

#define MAX_BOUNDARY    100
#define INIT_TEMP       20

/* 
  function: checkArgs
  ensures the command line argument fits requirements
*/
unsigned checkArgs(const char * arg) 
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
  
*/
__global__ void calculateTemp(double * h, double * g, unsigned N)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if ((i % (N - 1)) != 0 && (i % N) != 0 && i > N && i < (N * (N - 1)))
    {
        g[i] = 0.25 * (h[i- 1] + h[i + 1] + h[i - N] + h[i + N]);
    }
}

/* 
  function: copyMatrix
  
*/
__global__ void copyMatrix(double * h, double * g, unsigned N)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    h[i] = g[i];
}

/* function: main
   
*/
int main(int argc, char * argv[])
{
    /* check arguments */
    unsigned N = 100;
    unsigned I = 1;

    // for(int i = 1; i < argc; i++)
    // {

    // }
    
    /* placeholder for error checking */
    cudaError_t err = cudaSuccess;

    /* allocate cpu mem to array */
    size_t size = N * N * sizeof(double);
    int numElements = (N - 2) * (N - 2);
    double * H_h = (double *) malloc(size);
    double * H_g = (double *) malloc(size);

    /* verify that allocation succeeded */
    if (H_h == NULL || H_g == NULL) 
    {
        std::cout << "Failed to allocate host matrices" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* initialize all array interior positions */
    for (int i = 0; i < (N * N - 1); i++)
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

    /* set small section to 100 celsius */
    for (int i = ((int) ); i < ((int) ); i++)
    {
        
    }

    /* allocate gpu mem to matrix H */
    double * D_h = NULL;
    err = cudaMalloc((void **) &D_h, size);

    // /* verify that allocation succeeded */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to allocate device matrix H - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* allocate gpu mem to matrix G */
    double * D_g = NULL;
    err = cudaMalloc((void **) &D_g, size);

    // /* verify that allocation succeeded */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to allocate device matrix G - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* send matrix H to gpu */
    err = cudaMemcpy(D_h, H_h, size, cudaMemcpyHostToDevice);

    // /* verify memcopy from host to device succeeded */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to copy matrix H from host to device - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* send matrix G to gpu */
    err = cudaMemcpy(D_g, H_g, size, cudaMemcpyHostToDevice);

    // /* verify memcopy from host to device succeeded */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to copy matrix G from host to device - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* initialize event timing variables */
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    /* get device properties to set num of threads and blocks */
    cudaDeviceProp properties;
    cudaGetDeviceProperties(&properties, 0);
    int threadsPerBlock = properties.maxThreadsPerBlock;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

    /* execute kernel with in loop with timing */
    cudaEventRecord(start);
    while(I > 0)
    {
        calculateTemp<<<blocksPerGrid, threadsPerBlock>>>(D_h, D_g, N);
        cudaDeviceSynchronize();
        copyMatrix<<<blocksPerGrid, threadsPerBlock>>>(D_h, D_g, N);
        cudaDeviceSynchronize();
        I--;
    }
    cudaEventRecord(stop);

    /* synchronize event at end of kernel execution */
    cudaEventSynchronize(stop);

    // /* verify successful launch */
    // err = cudaGetLastError();

    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to launch calculateTemp kernel - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* calculate kernel execution time & write to console */
    float milli = 0;
    cudaEventElapsedTime(&milli, start, stop);
    std::cout << std::fixed << std::setprecision(2) << milli << std::endl;

    /* copy data back to cpu */
    err = cudaMemcpy(H_h, D_h, size, cudaMemcpyDeviceToHost);

    // /* verify memcopy from device to host succeeded */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to copy matrix H from device to host - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* evaluate results & write to csv */
    std::ofstream ofs ("finalTemperatures.csv", std::ofstream::trunc);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            ofs << std::fixed << std::setprecision(6) << *(H_h + (j * N) + i) << ", ";
        }
        ofs << std::endl;
    }
    ofs.close();

    /* free device memory */
    err = cudaFree(D_h);
    
    // /* verify cuda memory successfully freed */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to free device matrix/matrices - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    /* free device memory */
    err = cudaFree(D_g);
    
    // /* verify cuda memory successfully freed */
    // if (err != cudaSuccess) 
    // {
    //     std::cout << "Failed to free device matrix/matrices - " << cudaGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }
    
    /* free host memory */
    free(H_h);
    free(H_g);

    /* return successfully */
    exit(EXIT_SUCCESS);
}