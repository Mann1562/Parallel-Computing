#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#define ROCK 0
#define PAPER 1
#define SCISSORS 2

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4) {
        if (rank == 0) {
            printf("Error: Number of processes must be 4.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    srand(time(NULL) + rank); // Seed the random number generator

    if (rank == 0) {
        if (argc != 2) {
            printf("Usage: mpirun -np 4 %s <iterations>\n", argv[0]);
            MPI_Finalize();
            return EXIT_FAILURE;
        }

        int iterations = atoi(argv[1]);
        printf("MAIN: Rock, Paper, Scissors: %d iterations\n", iterations);

        // Start referee process
        MPI_Send(&iterations, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Wait for referee and players to finish
        MPI_Barrier(MPI_COMM_WORLD);
    } else if (rank == 1) { // Referee
        int iterations;
        MPI_Recv(&iterations, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int player1_score = 0, player2_score = 0;
        char *choices[] = {"Rock", "Paper", "Scissors"};

        for (int i = 1; i <= iterations; i++) {
            printf("REF: Go Players (%d)\n", i);

            // Send "READY" to players
            MPI_Send("READY", strlen("READY") + 1, MPI_CHAR, 2, 0, MPI_COMM_WORLD);
            MPI_Send("READY", strlen("READY") + 1, MPI_CHAR, 3, 0, MPI_COMM_WORLD);

            // Receive choices from players
            char choice1[20], choice2[20];
            MPI_Recv(choice1, 20, MPI_CHAR, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(choice2, 20, MPI_CHAR, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            printf("REF: Player 1: %s\n", choice1);
            printf("REF: Player 2: %s\n", choice2);

            // Determine winner
            if (strcmp(choice1, choice2) == 0) {
                printf("REF: Matching choices draw\n");
            } else if ((strcmp(choice1, "Rock") == 0 && strcmp(choice2, "Scissors") == 0) ||
                       (strcmp(choice1, "Paper") == 0 && strcmp(choice2, "Rock") == 0) ||
                       (strcmp(choice1, "Scissors") == 0 && strcmp(choice2, "Paper") == 0)) {
                printf("REF: Player 1 wins\n");
                player1_score++;
            } else {
                printf("REF: Player 2 wins\n");
                player2_score++;
            }
        }

        printf("REF: Tournament Ends\n");
        if (player1_score > player2_score) {
            printf("REF: Player 1 Wins (%d/%d)\n", player1_score, iterations);
        } else if (player1_score < player2_score) {
            printf("REF: Player 2 Wins (%d/%d)\n", player2_score, iterations);
        } else {
            printf("REF: Draw\n");
        }

        // Send "STOP" to players
        MPI_Send("STOP", strlen("STOP") + 1, MPI_CHAR, 2, 0, MPI_COMM_WORLD);
        MPI_Send("STOP", strlen("STOP") + 1, MPI_CHAR, 3, 0, MPI_COMM_WORLD);

        // Notify main process and terminate
        MPI_Barrier(MPI_COMM_WORLD);
    } else { // Players
        char choice[20];
        while (1) {
            MPI_Recv(choice, 20, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (strcmp(choice, "STOP") == 0) {
                break;
            }

            // Make a random choice
            int rand_choice = rand() % 3;
            switch (rand_choice) {
                case ROCK:
                    strcpy(choice, "Rock");
                    break;
                case PAPER:
                    strcpy(choice, "Paper");
                    break;
                case SCISSORS:
                    strcpy(choice, "Scissors");
                    break;
            }

            // Send choice to referee
            MPI_Send(choice, strlen(choice) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        }

        // Notify main process and terminate
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
