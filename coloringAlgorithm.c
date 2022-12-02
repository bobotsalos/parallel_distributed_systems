#include "./include/readFiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct queue {
    int *items;
    int front;
    int rear;
};

struct queue* createQueue(int N) {
    // printf("create\n");
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    q->items = (int*)malloc(N * sizeof(int));
    q->front=-1;
    q->rear=-1;
    return q;
}

int isEmpty(struct queue* q) {
    return q->rear == -1 ? 1 : 0; 
}

void enqueue(struct queue *q, int N, int v) {
    if(q->rear == N - 1) {
        printf("Queue is full!");
    } else {
        if(q->front == -1)
            q->front++;
        q->rear++;
        q->items[q->rear] = v;
    }
}

int dequeue(struct queue *q) {
    int item;
    if(isEmpty(q)) {
        printf("Queue is empty!");
        return 0;
    } else {
        item = q->items[q->front];
        q->front++;
        if(q->front > q->rear) {
            // printf("Resseting queue\n");
            q->front = q->rear = -1;
        }
    }
    return item;
}

void printQueue(struct queue *q) {
    if(isEmpty(q)) {
        printf("Queue is empty!");
    } else {
        printf("\nQueue contains: ");
        for(int i = q->front; i < q->rear+1; i++) {
            printf("%d ", q->items[i]);
        }
    }
}

// void BFS(int s, bool visited[SIZE], int **graph) {
//     struct queue *q = createQueue();

//     visited[s] = true;
//     enqueue(q, s);

//     while(!isEmpty(q)) {
//         printQueue(q);
//         int currentVertex = dequeue(q);
//         printf("\nVisited: %d", currentVertex);
        
//         int *adjList = graph[currentVertex];

//         for(int adjVertex = 0; adjVertex < SIZE; adjVertex++) {
//             if(adjList[adjVertex] == 1 && visited[adjVertex] == false) {
//                 visited[adjVertex] = true;
//                 enqueue(q, adjVertex);
//             }
//         }
//     }
// }

// int** calcTraspose(int **graph) {
//     int **tGraph = malloc(SIZE * sizeof(int*));
//     for(int i = 0; i < SIZE; i++) {
//         tGraph[i] = malloc(SIZE * sizeof(int));
//     }
//     for(int i = 0; i < SIZE; i++) {
//         for(int j = 0; j < SIZE; j++) {
//             tGraph[i][j] = graph[j][i];
//             tGraph[j][i] = graph[i][j];
//         }
//     }
//     return tGraph;
// }


// Coloring Algorithm
int coloringAlgorithm(int **mtx_csr, int **mtx_csc, int N) {
    struct queue *q = createQueue(N);
    int *colors = (int*) malloc(N * sizeof(int));
    int *visited = (int*) malloc(N * sizeof(int));
    int *scc_ids = (int*)malloc(N * sizeof(int));

    for(int i = 0; i < N; i++) {
        enqueue(q, N, i);
        scc_ids[i] = -1;
        visited[i] = false;
    }

    // printQueue(q);
    while(!isEmpty(q)){
        // printf("\nCOLORS: ");
        for(int i = q->front; i < q->rear+1; i++) {
            int v = q->items[i];
            colors[v] = v;
            // visited[v] = false;
            // printf("%d ", colors[v]);
        }
        // printf("\n");

        bool color_changed = true;
        while(color_changed) {
            color_changed = false;
            for(int i = 0; i < q->rear+1; i++) {
                int v = q->items[i];
                visited[v] = true;
                // printf("v: %d\n", v);

                // pass color to vertices v points
                int dsc_start = mtx_csr[0][v];
                int dsc_end = mtx_csr[0][v + 1];
                // printf("start: %d end: %d\n", dsc_start, dsc_end);
                for(int dsc_id = dsc_start; dsc_id < dsc_end; dsc_id++) {
                    int dsc = mtx_csr[1][dsc_id];
                    // printf("dsc: %d\n", dsc);
                    if(visited[dsc] == false) {
                        colors[dsc] = colors[v];
                        color_changed = true;
                    }
                }

                int pred_start = mtx_csc[1][v];
                int pred_end = mtx_csc[1][v + 1];
                for(int u_id = pred_start; u_id < pred_end; u_id++) {
                    int u = mtx_csc[0][u_id];
                    if(scc_ids[u] == -1 && colors[v] > colors[u]) {
                        colors[v] = colors[u];
                        color_changed = true;
                    }
                }
                // printf("COLORS: ");
                // for(int i = q->front; i < q->rear+1; i++ ){
                //     printf("%d ", colors[q->items[i]]);
                // }
                // printf("\n");
            }
        }

        for(int i = 0; i < q->rear+1; i++) {
            int v = q->items[i];
            if(colors[v] == v) {
                int scc_color = colors[v];
                struct queue *bfs_q = createQueue(N);
                enqueue(bfs_q, N, v);
                
                scc_ids[v] = scc_color;
                while(!isEmpty(bfs_q)) {
                    // printQueue(bfs_q);
                    int curr_vertex = dequeue(bfs_q);

                    int pred_start = mtx_csc[1][curr_vertex];
                    int pred_end = mtx_csc[1][curr_vertex + 1];
                    for(int u_id = pred_start; u_id < pred_end; u_id++) {
                        int u = mtx_csc[0][u_id];
                        if(scc_ids[u] == -1 && colors[u] == scc_color) {
                            // printf("%d\n", u);
                            enqueue(bfs_q , N, u);
                            scc_ids[u] = scc_color;
                        }
                    }
                    // printQueue(bfs_q);
                    // printf("\n");
                }
            }
        }
        for(int i = 0; i < q->rear+1; i++) {
            int v = q->items[i];
            if(scc_ids[v] != -1) {
                if(i == q->front) {
                    q->front++;
                    // printf("front: %d", q->front);
                } else if(i == q->rear) {
                    q->rear--;
                } else {
                    for(int j = i; j < q->rear; j++) {
                        // printf("v: %d\n", v);
                        q->items[j] = q->items[j+1];
                    }
                    q->rear--;
                }
                if(q->front > q->rear) {
                    // printf("Resseting queue\n");
                    q->front = q->rear = -1;
                }
            }
        }
        // printf("\n");;
    }
    // for(int i = 0; i < N; i++) {
    //     printf("%d ", scc_ids[i]);
    // }

    int cnt = 0;
    int *scc_cnt = (int*)malloc(N * sizeof(int));
    for(int i = 0; i < N; i++) {
        scc_cnt[i] = 0;
    }
    for(int i = 0; i < N; i++) {
        if(scc_cnt[scc_ids[i]] == 0) {
            scc_cnt[scc_ids[i]]++;
            cnt++;
        } else {
            scc_cnt[scc_ids[i]]++;
        }
    }
    //******* TRIM na kanoume prin to proccess ******************/
    
    // for(int i = 0; i < N; i++) {
    //     if(scc_cnt[scc_ids[i]] == 1) {
    //         cnt--;
    //     }
    // }
    // printf("%d ", scc_ids[i]);

    free(scc_cnt);
    free(scc_ids);
    free(q->items);
    free(q);

    return cnt;
}
// #define nz 11
// #define M 7
// #define N 7

// int main()
// {
//     MM_typecode matcode;
//     int *I = (int*)malloc(nz * sizeof(int));
//     int *J = (int*)malloc(nz * sizeof(int));                       
    
//     I[0] = 5;
//     I[1] = 0;
//     I[2] = 1;
//     I[3] = 3;
//     I[4] = 5;
//     I[5] = 2;
//     I[6] = 1;
//     I[7] = 6;
//     I[8] = 1;
//     I[9] = 3;
//     I[10] = 4;

//     J[0] = 0;
//     J[1] = 1;
//     J[2] = 2;
//     J[3] = 2;
//     J[4] = 2;
//     J[5] = 3;
//     J[6] = 4;
//     J[7] = 4;
//     J[8] = 5;
//     J[9] = 6;
//     J[10] = 6;

//     FILE *fptr;
//     fptr = fopen("./graph.mtx", "w+");
//     if(fptr == NULL) {
//         printf("Error!");   
//         exit(1);  
//     }

//     mm_initialize_typecode(&matcode);
//     mm_set_matrix(&matcode);
//     mm_set_coordinate(&matcode);
//     mm_set_real(&matcode);

//     mm_write_banner(fptr, matcode); 
//     mm_write_mtx_crd_size(fptr, M, N, nz);
//     /* NOTE: matrix market files use 1-based indices, i.e. first element
//       of a vector has index 1, not 0.  */


//     for (int i=0; i<nz; i++)
//         fprintf(fptr, "%d %d\n", I[i]+1, J[i]+1);

//     fclose(fptr);
// 	return 0;
// }

int main(int argc, char *argv[]) {
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i, *I, *J, *val;
    int ret_code;

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}
    else    
    { 
        if ((f = fopen(argv[1], "r")) == NULL) 
            exit(1);
    }

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }
        
    /* find out size of sparse matrix */
    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0)
        exit(1);
    /* reserve memory for matrices */
    // int nz = 11;
    // int N = 7;
    I = (int*)malloc(nz * sizeof(int));
    J = (int*)malloc(nz * sizeof(int));
    val = (int*)malloc(nz * sizeof(int));

    for (int i = 0; i < nz; i++)
    {
        fscanf(f, "%d %d\n", &I[i], &J[i]); /* don't read values (or weights) */
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;  /* adjust from 1-based to 0-based */
    }

    if (f != stdin) 
        fclose(f);


    // I[0] = 5;
    // I[1] = 0;
    // I[2] = 1;
    // I[3] = 3;
    // I[4] = 5;
    // I[5] = 2;
    // I[6] = 1;
    // I[7] = 6;
    // I[8] = 1;
    // I[9] = 3;
    // I[10] = 4;

    // J[0] = 0;
    // J[1] = 1;
    // J[2] = 2;
    // J[3] = 2;
    // J[4] = 2;
    // J[5] = 3;
    // J[6] = 4;
    // J[7] = 4;
    // J[8] = 5;
    // J[9] = 6;
    // J[10] = 6;
    
    int **mtx_csr = graph_csr((int*)I, (int*)J, nz, N);
    int **mtx_csc = graph_csc((int*)I, (int*)J, nz, N);
    int sccs = coloringAlgorithm(mtx_csr, mtx_csc, N);
    printf("sccs: %d\n", sccs);

    free(mtx_csr[1]);
    free(mtx_csr[0]);
    free(mtx_csc[1]);
    free(mtx_csc[0]);
    free(mtx_csc);
    free(mtx_csr);
    free(I);
    free(J);

    return 0;
}