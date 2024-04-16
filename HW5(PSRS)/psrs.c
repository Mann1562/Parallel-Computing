// make to create the psrs executable
// once psrs is created you can run mpirun -np 2 ./psrs with 2 being the number of processors (can be varies)
// you will need to enter the size i.e. 'n' at the prompt
// make clean to remove

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0

// Function to compare integers (for qsort)
int compare_integers(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Function to perform PSRS algorithm
void psrs(int *data, int n, int *sorted_data) {
    // Step 1: Local sorting
    qsort(data, n, sizeof(int), compare_integers);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int *data = NULL;
    int *sorted_data = NULL;
    int n; // Example problem size

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == MASTER) {
        printf("Enter the size of the problem (n): ");
        fflush(stdout); // Flush the buffer to ensure prompt is displayed immediately
        scanf("%d", &n);

        data = (int *)malloc(n * sizeof(int));
        // Initialize or read data
        for (int i = 0; i < n; i++) {
            data[i] = rand() % 1000;
        }
    }

    // Broadcast the size of the problem to all processes
    MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Allocate memory for scattered data
    int *scattered_data = (int *)malloc(n / size * sizeof(int));

    // Scatter data to each processor
    MPI_Scatter(data, n / size, MPI_INT, scattered_data, n / size, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Sort scattered data individually on each processor
    qsort(scattered_data, n / size, sizeof(int), compare_integers);

    // Allocate memory for sorted data on master process
    if (rank == MASTER) {
        sorted_data = (int *)malloc(n * sizeof(int));
    }

    // Gather sorted data to master process
    MPI_Gather(scattered_data, n / size, MPI_INT, sorted_data, n / size, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Merge sorted data on master process
    if (rank == MASTER) {
        // Perform PSRS algorithm on merged sorted data
        psrs(sorted_data, n, sorted_data);

        // Output sorted data
        printf("Sorted Data: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", sorted_data[i]);
        }
        printf("\n");

        free(data);
        free(sorted_data);
    }

    free(scattered_data);
    MPI_Finalize();
    return 0;
}
