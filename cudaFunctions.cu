/*
 * cudaFunctions.cu
 *
 *  Created on: Jun 4, 2012
 *      Author: petr
 */

#include <cuda.h>
#include "cudaFunctions.h"

__global__ void makeSumOfHashOnDevice(unsigned* hashPart2Sha1) {
    int i = threadIdx.x;

    if( i > 0 && i < HASH_SIZE ) {
        hashPart2Sha1[0] += hashPart2Sha1[i];
    }

}

void cudaMakeSumOfHash(unsigned* hashPart2Sha1) {

    unsigned* d_A;
    int size = sizeof(unsigned) * HASH_SIZE;

    cudaMalloc(&d_A, size);
    cudaMemcpy(d_A, hashPart2Sha1, size, cudaMemcpyHostToDevice);

    makeSumOfHashOnDevice<<<1, HASH_SIZE>>>(d_A);

    cudaMemcpy(hashPart2Sha1, d_A, size, cudaMemcpyDeviceToHost);

    cudaFree(d_A);
}
