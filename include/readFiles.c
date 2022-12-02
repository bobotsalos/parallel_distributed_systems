#include "readFiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmio.h"

void print_graph(MM_typecode matcode, int M, int N, int nz, int** graph) {
    /************************/
    /* print out matrix     */
    /************************/
    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
    for (int i=0; i<nz; i++)
        fprintf(stdout, "%d\t %d\n", graph[0][i], graph[1][i]);
}

int get_length(FILE* f) {
    int M, N, length;
    /* find out size of sparse matrix */
    if (mm_read_mtx_crd_size(f, &M, &N, &length) != 0)
        exit(1);
    
    return length;
}

int** read_dot_mtx(FILE* f, MM_typecode matcode) {
    /*
    DESC
    ----------
    find size of sparse Matrix, read the matrix

    Parameters
    ----------
    FILE* f -> .mtx file open with read access

    Returns
    ----------
    a 2D array that contains
    at array[0] -> rows
    at array[1] -> col
    at array[2] -> weights (for us all the weights are equal to 1)
    */

    int M, N, nz;   
    int i, *I, *J;
    int ret_code;

        
    /* find out size of sparse matrix */
    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0)
        exit(1);

    /* reserve memory for matrices */
    I = malloc(nz * sizeof(int*));
    J = malloc(nz * sizeof(int*));

    for (int i = 0; i < nz; i++)
    {
        fscanf(f, "%d %d %*s\n", &I[i], &J[i]); /* don't read values (or weights) */
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;  /* adjust from 1-based to 0-based */
    }

    if (f != stdin) fclose(f);

    int** graph = malloc(2 * sizeof(int**));
    graph[0] = I;
    graph[1] = J;

    // print_graph(matcode, M, N, nz, graph);

    return graph;
}

int** graph_csc(int* row_coo, int* col_coo, int nnz, int N) {
    int *row_csc = (int*)malloc(nnz * sizeof(int));
    int *col_csc = (int*)malloc((N+1) * sizeof(int));

    // initialize to 0
    for(int i = 0; i < N+1; i++) {
        col_csc[i] = 0;
    }

    // find the correct column sizes
    for(int i = 0; i < nnz; i++) {
        col_csc[col_coo[i]]++;
    }

    // cumulative sum
    for(int i = 0, cum_sum=0; i < N; i++) {
        int temp = col_csc[i];
        col_csc[i] = cum_sum;
        cum_sum += temp;
    }
    col_csc[N] = nnz;

    // copy row indicies to the correct place
    for(int i = 0; i < nnz; i++) {
        int col_l = col_coo[i];

        int dst = col_csc[col_l];
        row_csc[dst] = row_coo[i];

        col_csc[col_l]++;
    }

    // revert the csc column vector
    for(int i = 0, last = 0; i < N; i++) {
        int temp = col_csc[i];
        col_csc[i] = last;
        last = temp;
    }

    int** graph = (int**)malloc(2 * sizeof(int*));
    graph[0] = (int*)malloc(nnz * sizeof(int));
    graph[1] = (int*)malloc((N+1) * sizeof(int));
    graph[0] = row_csc;
    graph[1] = col_csc;

    return graph;
}

int** graph_csr(int *row_coo, int *col_coo, int nnz, int N) {
    int *row_csr = (int*)malloc((N+1) * sizeof(int));
    int *col_csr = (int*)malloc(nnz * sizeof(int));

    // initialize to 0
    for(int i = 0; i < N+1; i++) {
        row_csr[i] = 0;
    }

    // find the correct row sizes
    for(int i = 0; i < nnz; i++) {
        row_csr[row_coo[i]]++;
    }

    // cumulative sum
    for(int i = 0, cum_sum = 0; i < N; i++) {
        int temp = row_csr[i];
        row_csr[i] = cum_sum;
        cum_sum += temp;
    }
    row_csr[N] = nnz;
    // copy col indicies to the correct place
    for(int i = 0; i < nnz; i++) {
        int row_l = row_coo[i];

        int dst = row_csr[row_l];
        col_csr[dst] = col_coo[i];

        row_csr[row_l]++;
    }

    // revert the csc row vector
    for(int i = 0, last = 0; i < N; i++) {
        int temp = row_csr[i];
        row_csr[i] = last;
        last = temp;
    }

    int** graph = (int**)malloc(2 * sizeof(int*));
    graph[0] = (int*)malloc((N+1) * sizeof(int));
    graph[1] = (int*)malloc(nnz * sizeof(int));
    graph[0] = row_csr;
    graph[1] = col_csr;

    return graph;
}


// int main(int argc, char* argv[]) {
//     MM_typecode matcode;
//     FILE *f;
//     int M, N, nz;   
//     int i, *I, *J;
//     int ret_code;

//     if (argc < 2)
// 	{
// 		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
// 		exit(1);
// 	}
//     else    
//     { 
//         if ((f = fopen(argv[1], "r")) == NULL) 
//             exit(1);
//     }

//     if (mm_read_banner(f, &matcode) != 0)
//     {
//         printf("Could not process Matrix Market banner.\n");
//         exit(1);
//     }
        
//     /* find out size of sparse matrix */
//     if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0)
//         exit(1);

//     /* reserve memory for matrices */
//     I = malloc(nz * sizeof(int*));
//     J = malloc(nz * sizeof(int*));

//     for (int i = 0; i < nz; i++)
//     {
//         fscanf(f, "%d %d %*s\n", &I[i], &J[i]); /* don't read values (or weights) */
//         I[i]--;  /* adjust from 1-based to 0-based */
//         J[i]--;  /* adjust from 1-based to 0-based */
//     }

//     if (f != stdin) fclose(f);

//     int** cmp = graph_csc(I, J, nz, N);

//     print_graph(matcode, M, N, nz, cmp);

//     free(cmp[0]);
//     free(cmp[1]);
//     free(cmp);
//     return 0;
// }