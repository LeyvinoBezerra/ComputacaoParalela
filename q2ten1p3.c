#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verifica se há exatamente 5 processos
    if (size != 5) {
        if (rank == 0) {
            fprintf(stderr, "Erro: este programa requer exatamente 5 processos. Use: mpirun -np 5 ./mpi_exemplo\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        // MPI_Finalize não é alcançado após Abort, mas deixamos por clareza
        MPI_Finalize();
        return 1;
    }

    const int TAG_PERM = 10;   // P0 -> P1 (permissão)
    const int TAG_VAL  = 20;   // P1 -> P3 (valor)
    const int TAG_RES  = 30;   // P3 -> P4 (resultado)
    const int TAG_OK   = 40;   // P2 -> P4 (liberação)

    int valor = rank;

    if (rank == 0) {
        int perm = 1;
        MPI_Send(&perm, 1, MPI_INT, 1, TAG_PERM, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        int perm;
        MPI_Recv(&perm, 1, MPI_INT, 0, TAG_PERM, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&valor, 1, MPI_INT, 3, TAG_VAL, MPI_COMM_WORLD);
    }

    if (rank == 3) {
        int v1;
        MPI_Recv(&v1, 1, MPI_INT, 1, TAG_VAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int soma = v1 + valor; // 1 + 3 = 4
        MPI_Send(&soma, 1, MPI_INT, 4, TAG_RES, MPI_COMM_WORLD);
    }

    if (rank == 2) {
        int ok = 1;
        MPI_Send(&ok, 1, MPI_INT, 4, TAG_OK, MPI_COMM_WORLD);
    }

    if (rank == 4) {
        int result, ok;
        MPI_Recv(&result, 1, MPI_INT, 3, TAG_RES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&ok, 1, MPI_INT, 2, TAG_OK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Resultado final em P4: %d\n", result);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
