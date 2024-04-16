#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cstdlib>

// CUDA Kernel to compute dot product
__global__ void dotProduct(double *a, double *b, double *partialSums, int n) {
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    int stride = blockDim.x * gridDim.x;

    __shared__ double sharedMemory[256]; // Shared memory for partial dot products

    sharedMemory[threadIdx.x] = 0; // Initialize shared memory

    // Compute partial dot products in parallel
    for (int i = index; i < n; i += stride) {
        sharedMemory[threadIdx.x] += a[i] * b[i];
    }

    __syncthreads(); // Ensure all threads have finished updating shared memory

    // Reduction in shared memory to compute partial sums
    for (int i = blockDim.x / 2; i > 0; i >>= 1) {
        if (threadIdx.x < i) {
            sharedMemory[threadIdx.x] += sharedMemory[threadIdx.x + i];
        }
        __syncthreads(); // Synchronize threads after each reduction step
    }

    // Write the partial sum to global memory
    if (threadIdx.x == 0) {
        partialSums[blockIdx.x] = sharedMemory[0];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <numBlocks> <threadsPerBlock>" << std::endl;
        return 1;
    }

    int numBlocks = atoi(argv[1]);
    int threadsPerBlock = atoi(argv[2]);

    int n = 1 << 18; // Length of vectors 'a' and 'b'
    std::vector<double> vectorA(n); // Initialize vector 'a' with random values
    std::vector<double> vectorB(n); // Initialize vector 'b' with random values

    // Fill vectors with random values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(1.0, 10.0); // Generate random values between 1 and 10
    for (int i = 0; i < n; ++i) {
        vectorA[i] = dis(gen);
        vectorB[i] = dis(gen);
    }

    double *deviceA, *deviceB, *devicePartialSums; // Device pointers
    double *partialSums = new double[numBlocks]; // Array to store partial sums on the host
    double result = 0.0; // Variable to store the dot product result

    cudaError_t cudaStatus; // Variable to store CUDA function return status

    // Allocate device memory for vectors 'a', 'b', and partial sums
    cudaStatus = cudaMalloc(&deviceA, n * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaMalloc failed for deviceA!" << std::endl;
        return 1;
    }

    cudaStatus = cudaMalloc(&deviceB, n * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaMalloc failed for deviceB!" << std::endl;
        return 1;
    }

    cudaStatus = cudaMalloc(&devicePartialSums, numBlocks * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaMalloc failed for devicePartialSums!" << std::endl;
        return 1;
    }

    // Copy input data from host to device memory
    cudaMemcpy(deviceA, vectorA.data(), n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(deviceB, vectorB.data(), n * sizeof(double), cudaMemcpyHostToDevice);

    // Benchmark CUDA implementation
    auto start = std::chrono::high_resolution_clock::now();
    dotProduct<<<numBlocks, threadsPerBlock>>>(deviceA, deviceB, devicePartialSums, n);
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaDeviceSynchronize returned error code " << cudaStatus << " after launching dotProduct kernel!" << std::endl;
        return 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Num Blocks: " << numBlocks << ", Threads Per Block: " << threadsPerBlock << ", CUDA time: " << duration.count() << " seconds" << std::endl;

    // Copy partial sums from device to host
    cudaMemcpy(partialSums, devicePartialSums, numBlocks * sizeof(double), cudaMemcpyDeviceToHost);

    // Compute final dot product on the host
    for (int i = 0; i < numBlocks; ++i) {
        result += partialSums[i];
    }

    // Print final dot product
    std::cout << "CUDA dot product: " << result << std::endl;

    // Free device memory
    cudaFree(deviceA);
    cudaFree(deviceB);
    cudaFree(devicePartialSums);
    delete[] partialSums;

    return 0;
}
