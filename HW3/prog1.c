#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    // local variables
    int size, rank, name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    // (a) Initialize MPI
    MPI_Init(&argc, &argv);

    // (b) Get the size and rank for each process
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // (c) Build a character string that contains the process rank, size, and processor name
    MPI_Get_processor_name(processor_name, &name_len);
    char char_string[100];
    sprintf(char_string, "Greetings from process %d of %d on %s", rank, size, processor_name);

    // (d) Barrier
    MPI_Barrier(MPI_COMM_WORLD);

    // (e) Create a for-loop in which process 0 receives p-1 messages from processes [1..p-1] in order and prints out the above string. Note only process 0 will output the messages received.

    MPI_Send(char_string, strlen(char_string) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Part1\n");
        for (int i = 0; i < size; ++i) {
            MPI_Recv(char_string, 100, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", char_string);
        }
        printf("\n");
    }

    // (f) Barrier
    MPI_Barrier(MPI_COMM_WORLD);

    // (g) Create a for-loop in which process 0 receives p-1 messages from processes [1..p-1] in any order and prints out the above string. Note only process 0 will output the messages received
    
    MPI_Send(char_string, strlen(char_string) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Part2\n");
        for (int i = 0; i < size; ++i) {
            MPI_Recv(char_string, 100, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", char_string);
        }
        printf("\n");
    }

    // Finalize MPI
    MPI_Finalize();

    // Exit
    return 0;
}