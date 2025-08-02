#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Node structure for adjacency list
typedef struct Node {
    int id;
    struct Node* next;
} Node;

// Graph structure
typedef struct Graph {
    int numVertices;
    Node** adjLists;
} Graph;

// Queue structure for BFS (finding connections)
typedef struct QueueNode {
    int vertex;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

// Function prototypes
Graph* createGraph(int vertices);
void addEdge(Graph* graph, int src, int dest);
void loadGraphFromFile(Graph** graph, char* filename);
void displayFriendList(Graph* graph, int personId);
void findConnection(Graph* graph, int src, int dest);
void displayMenu();
void freeGraph(Graph* graph);

// Queue operations for BFS
Queue* createQueue();
void enqueue(Queue* q, int vertex);
int dequeue(Queue* q);
bool isEmpty(Queue* q);
void freeQueue(Queue* q);

// Helper function to add node to adjacency list
Node* createNode(int id) {
    Node* newNode = malloc(sizeof(Node));
    newNode->id = id;
    newNode->next = NULL;
    return newNode;
}

// Create graph with given number of vertices
Graph* createGraph(int vertices) {
    Graph* graph = malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->adjLists = malloc(vertices * sizeof(Node*));
    
    // Initialize all adjacency lists as empty
    for (int i = 0; i < vertices; i++) {
        graph->adjLists[i] = NULL;
    }
    
    return graph;
}

// Add edge to undirected graph (bidirectional friendship)
void addEdge(Graph* graph, int src, int dest) {
    // Add edge from src to dest
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
    
    // Add edge from dest to src (since friendship is bidirectional)
    newNode = createNode(src);
    newNode->next = graph->adjLists[dest];
    graph->adjLists[dest] = newNode;
}

// Load graph from file
void loadGraphFromFile(Graph** graph, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }
    
    int n, e;
    fscanf(file, "%d %d", &n, &e);
    
    *graph = createGraph(n);
    
    // Read edges
    for (int i = 0; i < e; i++) {
        int src, dest;
        fscanf(file, "%d %d", &src, &dest);
        addEdge(*graph, src, dest);
    }
    
    fclose(file);
    printf("Graph loaded successfully!\n");
}

// Display friend list for a given person
void displayFriendList(Graph* graph, int personId) {
    if (personId < 0 || personId >= graph->numVertices) {
        printf("Error: Person ID %d does not exist in the dataset.\n", personId);
        return;
    }
    
    Node* temp = graph->adjLists[personId];
    int friendCount = 0;
    
    printf("Person %d has ", personId);
    
    // Count friends first
    Node* counter = temp;
    while (counter != NULL) {
        friendCount++;
        counter = counter->next;
    }
    
    printf("%d friends!\n", friendCount);
    
    if (friendCount > 0) {
        printf("List of friends: ");
        while (temp != NULL) {
            printf("%d", temp->id);
            if (temp->next != NULL) {
                printf(" ");
            }
            temp = temp->next;
        }
        printf("\n");
    }
}

// Queue operations for BFS
Queue* createQueue() {
    Queue* q = malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue* q, int vertex) {
    QueueNode* newNode = malloc(sizeof(QueueNode));
    newNode->vertex = vertex;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    
    q->rear->next = newNode;
    q->rear = newNode;
}

int dequeue(Queue* q) {
    if (q->front == NULL) return -1;
    
    QueueNode* temp = q->front;
    int vertex = temp->vertex;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    return vertex;
}

bool isEmpty(Queue* q) {
    return q->front == NULL;
}

// Find connection between two people using BFS
void findConnection(Graph* graph, int src, int dest) {
    if (src < 0 || src >= graph->numVertices || dest < 0 || dest >= graph->numVertices) {
        printf("Error: One or both person IDs do not exist in the dataset.\n");
        return;
    }
    
    if (src == dest) {
        printf("Person %d is the same as person %d.\n", src, dest);
        return;
    }
    
    // BFS to find shortest path
    bool* visited = malloc(graph->numVertices * sizeof(bool));
    int* parent = malloc(graph->numVertices * sizeof(int));
    
    for (int i = 0; i < graph->numVertices; i++) {
        visited[i] = false;
        parent[i] = -1;
    }
    
    Queue* queue = createQueue();
    visited[src] = true;
    enqueue(queue, src);
    
    bool found = false;
    
    while (!isEmpty(queue) && !found) {
        int current = dequeue(queue);
        
        Node* temp = graph->adjLists[current];
        while (temp != NULL) {
            int neighbor = temp->id;
            
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                parent[neighbor] = current;
                enqueue(queue, neighbor);
                
                if (neighbor == dest) {
                    found = true;
                    break;
                }
            }
            temp = temp->next;
        }
    }
    
    if (found) {
        printf("There is a connection from %d to %d!\n", src, dest);
        
        // Reconstruct path
        int* path = malloc(graph->numVertices * sizeof(int));
        int pathLength = 0;
        int current = dest;
        
        while (current != -1) {
            path[pathLength++] = current;
            current = parent[current];
        }
        
        // Print path in reverse order
        for (int i = pathLength - 1; i > 0; i--) {
            printf("%d is friends with %d\n", path[i], path[i-1]);
        }
        
        free(path);
    } else {
        printf("Cannot find a connection between %d and %d\n", src, dest);
    }
    
    free(visited);
    free(parent);
    freeQueue(queue);
}

void freeQueue(Queue* q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
    free(q);
}

// Display menu
void displayMenu() {
    printf("\nMAIN MENU\n");
    printf("[1] Get friend list\n");
    printf("[2] Get connection\n");
    printf("[3] Exit\n");
    printf("Enter your choice: ");
}

// Free graph memory
void freeGraph(Graph* graph) {
    if (graph == NULL) return;
    
    for (int i = 0; i < graph->numVertices; i++) {
        Node* current = graph->adjLists[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(graph->adjLists);
    free(graph);
}

// Main function
int main() {
    Graph* graph = NULL;
    char filename[256];
    int choice, personId, src, dest;
    
    char inputName[128];
    printf("Input file path: ");
    scanf("%s", inputName);
    snprintf(filename, sizeof(filename), "data/%s", inputName);

    // Read the filename/path directly
    loadGraphFromFile(&graph, filename);
    
    if (graph == NULL) {
        printf("Failed to load graph. Exiting.\n");
        return 1;
    }
    
    while (1) {
        displayMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Enter ID of person: ");
                scanf("%d", &personId);
                displayFriendList(graph, personId);
                break;
                
            case 2:
                printf("Enter ID of first person: ");
                scanf("%d", &src);
                printf("Enter ID of second person: ");
                scanf("%d", &dest);
                findConnection(graph, src, dest);
                break;
                
            case 3:
                printf("Goodbye!\n");
                freeGraph(graph);
                return 0;
                
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}