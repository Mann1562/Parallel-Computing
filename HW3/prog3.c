#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_MESSAGE_SIZE (128 * 1024) // Maximum message size in bytes

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0)
            printf("This program must be run with exactly 2 processes\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    double start_time, end_time;
    double latency, bandwidth;

    for (int msg_size = 1; msg_size <= MAX_MESSAGE_SIZE; msg_size *= 2) {
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            char* send_buffer = (char*)malloc(msg_size * sizeof(char));
            start_time = MPI_Wtime();
            MPI_Send(send_buffer, msg_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(send_buffer, msg_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            end_time = MPI_Wtime();
            
            latency = (end_time - start_time) / 2.0;
            bandwidth = msg_size / (end_time - start_time);
            
            printf("%d,%.6f,%.6f\n", msg_size, latency, bandwidth);
            free(send_buffer); // Free allocated memory
        } else if (rank == 1) {
            char* send_buffer = (char*)malloc(msg_size * sizeof(char));
            MPI_Recv(send_buffer, msg_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(send_buffer, msg_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
            free(send_buffer); // Free allocated memory
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
