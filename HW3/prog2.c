#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_STRING_LENGTH 100

int main(int argc, char *argv[]) {
    int my_rank;            // Rank of process
    int p;                  // Number of processes
    char original_string[MAX_STRING_LENGTH];
    char processed_string[MAX_STRING_LENGTH];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // Ensure we have exactly 3 processes
    if (p != 3) {
        if (my_rank == 0) {
            fprintf(stderr, "This program requires exactly 3 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Process 0 reads the string from standard input
    if (my_rank == 0) {
        printf("Please enter a string: ");
        fflush(stdout);
        scanf("%s", original_string);

        // Send the string to process 1
        MPI_Send(original_string, strlen(original_string) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

        // Receive the processed string from process 2
        MPI_Recv(processed_string, MAX_STRING_LENGTH, MPI_CHAR, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Processed string is: %s\n", processed_string);
    } 
    // Process 1 receives the string, reverses it, and sends it to process 2
    else if (my_rank == 1) {
        MPI_Recv(original_string, MAX_STRING_LENGTH, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int len = strlen(original_string);
        for (int i = 0; i < len; i++) {
            processed_string[i] = original_string[len - 1 - i];
        }
        processed_string[len] = '\0';

        MPI_Send(processed_string, strlen(processed_string) + 1, MPI_CHAR, 2, 0, MPI_COMM_WORLD);
    } 
    // Process 2 receives the reversed string, converts it to uppercase, and sends it back to process 0
    else if (my_rank == 2) {
        MPI_Recv(original_string, MAX_STRING_LENGTH, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int len = strlen(original_string);
        for (int i = 0; i < len; i++) {
            if (original_string[i] >= 'a' && original_string[i] <= 'z') {
                processed_string[i] = original_string[i] - 'a' + 'A';
            } else {
                processed_string[i] = original_string[i];
            }
        }
        processed_string[len] = '\0';

        MPI_Send(processed_string, strlen(processed_string) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
