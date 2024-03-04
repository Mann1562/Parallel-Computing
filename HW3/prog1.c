#include <stdio.h>
#include <mpi.h>
#include <string.h>

int main(int argc, char* argv[]) {
  int rank, size;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;

  // (a) Initialize MPI
  MPI_Init(&argc, &argv);

  // (b) Get the size and rank for each process
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // (c) Build a character string  
  MPI_Get_processor_name(processor_name, &name_len);
  name_len = name_len - 1; // Remove the trailing null character
  char message[256];
  sprintf(message, "Greetings from process %d of %d on %s\n", rank, size, processor_name);

  // Print separator before receiving messages
  if (rank == 0) {
    printf("PART1\n");
  }

  // (d) Barrier
  MPI_Barrier(MPI_COMM_WORLD);

  // (e) Ordered receive
  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      char recv_message[256];
      MPI_Recv(recv_message, 256, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("%s", recv_message);
    }
  } else {
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  }

  // (f) Barrier
  MPI_Barrier(MPI_COMM_WORLD);

  // Print separator before receiving messages (ensure process 0 sends)
  if (rank == 0) {
    printf("\nPART2\n");
  }
  // Make sure all processes send, even process 0
  MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

// (g) Unordered receive
if (rank == 0) {
  for (int i = 0; i < size - 1; i++) {
    char recv_message[256];
    int source = i % (size - 1) + 1; // Determine the source process based on loop index
    MPI_Recv(recv_message, 256, MPI_CHAR, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("%s", recv_message);
    }
} else {
  MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
}


  // (h) Finalize MPI
  MPI_Finalize();

  return 0;
}
