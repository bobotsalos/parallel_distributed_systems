#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 5

struct queue {
    int items[SIZE];
    int front;
    int rear;
};

struct queue* createQueue() {
    struct queue* q = malloc(sizeof(struct queue));
    q->front=-1;
    q->rear=-1;
    return q;
}

int isEmpty(struct queue* q) {
    return q->rear == -1 ? 1 : 0; 
}

void enqueue(struct queue *q, int v) {
    if(q->rear == SIZE - 1) {
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
        exit(1);
    } else {
        item = q->items[q->front];
        q->front++;
        if(q->front > q->rear) {
            printf("Resseting queue\n");
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
        for(int i = q->front; i < q->rear; i++) {
            printf("%d ", q->items[i]);
        }
    }
}

void BFS(int s, bool visited[SIZE], int **graph) {
    struct queue *q = createQueue();

    visited[s] = true;
    enqueue(q, s);

    while(!isEmpty(q)) {
        printQueue(q);
        int currentVertex = dequeue(q);
        printf("\nVisited: %d", currentVertex);
        
        int *adjList = graph[currentVertex];

        for(int adjVertex = 0; adjVertex < SIZE; adjVertex++) {
            if(adjList[adjVertex] == 1 && visited[adjVertex] == false) {
                visited[adjVertex] = true;
                enqueue(q, adjVertex);
            }
        }
    }
}

int** calcTraspose(int **graph) {
    int **tGraph = malloc(SIZE * sizeof(int*));
    for(int i = 0; i < SIZE; i++) {
        tGraph[i] = malloc(SIZE * sizeof(int));
    }
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            tGraph[i][j] = graph[j][i];
            tGraph[j][i] = graph[i][j];
        }
    }
    return tGraph;
}

int* coloringAlgorithm(int** graph) {
    struct queue *q = createQueue();
    int colors[SIZE];
    int scc_ids[SIZE];
    int** tGraph = calcTraspose(graph);

    for(int i = 0; i < SIZE; i++) {
        q->items[i] = i;
        scc_ids[i] = -1;
    }

    while(!isEmpty(q)){
        for(int i = 0; i < SIZE; i++) {
            colors[i] = i;
        }

        int color_changed = true;
        while(color_changed) {
            color_changed = false;
            for(int v = 0; v < SIZE; v++) {
                // pass color to vertices v points
                int *childs = graph[v];
                for(int i = 0; i < SIZE; i++) {
                    if(childs[i] == 1) {
                        colors[i] = colors[v];
                        color_changed = true;
                    }
                }
                int *predecessors = tGraph[v];
                for(int u = 0; u < SIZE; u++) {
                    if(predecessors[u] == 1) {
                        if(scc_ids[u] == -1 && colors[v] > colors[u]) {
                            colors[v] = colors[u];
                            color_changed = true;
                        }
                    }
                }
            }
        }

        for(int v = 0; v < SIZE; v++) {
            if(colors[v] == v) {
                int scc_color = colors[v];
                struct queue *bfs_q = createQueue();
                enqueue(bfs_q, v);
                
                while(!isEmpty(bfs_q)) {
                    printQueue(bfs_q);
                    int curr_vertex = dequeue(bfs_q);

                    int *predecessors = tGraph[curr_vertex];
                    for(int u = 0; u < SIZE; u++) {
                        if(predecessors[u] == 1) {
                            if(scc_ids[u] == -1 && colors[u] == scc_color) {
                                enqueue(bfs_q ,u);
                                scc_ids[u] = scc_color;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    int **graph = malloc(SIZE * sizeof(int*));
    for(int i = 0; i < SIZE; i++) {
        graph[i] = malloc(SIZE * sizeof(int));
        for(int j = 0; j < SIZE; j++) {
            graph[i][j] = 0;
        }
    }
    graph[0][2] = 1;
    graph[0][3] = 1;
    graph[1][0] = 1;
    graph[2][1] = 1;
    graph[3][4] = 1;

    coloringAlgorithm(graph);

    // bool visited[SIZE];
    // for(int i = 0; i < SIZE; i++){
    //     visited[i] = false;
    // }

    // BFS(0, visited, graph);
    return 0;
}