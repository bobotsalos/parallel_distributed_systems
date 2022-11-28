#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"

void print_graph(MM_typecode matcode, int M, int N, int nz, int** graph);
int get_length(FILE* f);
int** read_dot_mtx(FILE* f, MM_typecode matcode);
int** graph_csc(int *row_coo, int *col_coo,int nnz,int n);
int** graph_csr(int *row_coo, int *col_coo, int nnz, int N);

#endif