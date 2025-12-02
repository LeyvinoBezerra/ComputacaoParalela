#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int valor = rank;
    int recebido;

    if (rank == 0) {
        // P0 só manda permissão para P1
        MPI_Send(&valor, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        // P1 espera permissão de P0
        MPI_Recv(&recebido, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Depois manda seu valor para P3
        MPI_Send(&valor, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    }

    if (rank == 3) {
        // P3 recebe valor de P1
        MPI_Recv(&recebido, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int soma = valor + recebido;
        // Manda resultado para P4
        MPI_Send(&soma, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);
    }

    if (rank == 2) {
        // P2 manda sinal de liberação para P4
        MPI_Send(&valor, 1, MPI_INT, 4, 1, MPI_COMM_WORLD);
    }

    if (rank == 4) {
        // P4 recebe resultado de P3
        MPI_Recv(&recebido, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // P4 espera sinal de P2
        int sinal;
        MPI_Recv(&sinal, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Só imprime depois do sinal
        printf("Resultado final em P4: %d\n", recebido);
    }

    MPI_Finalize();
    return 0;
}
