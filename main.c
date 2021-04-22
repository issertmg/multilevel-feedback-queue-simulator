/*************************************************************** 
Names: GAGAN, Isser Troy M.
       MATIAS, Maria Angela Mikaela E.
Group: 40
Section: S15
***************************************************************/

#include <stdio.h>
#include <stdlib.h>

typedef struct Process {
  /* input*/
  int id;                     //F
  int arrival_time;           //G
  int total_execution_time;   //H
  int io_burst_time;          //I
  int io_frequency;           //J
  
  /* addtnl attributes */
  int execution_time_left;    //time left from total_execution_time
  int io_burst_timer;         //to check if process will withhold cpu already
  int io_burst_time_left;     //time left from io_burst_time
  int time_quantum_left;      //to check if process already consumed allocated time quantum
  int queue_index;

  /* output */
  int start_end_array_size;   //initialized to 0 (length of start_time and end_time array)
  int start_time[500];
  int end_time[500];
  char queue_id [500][10]; 
  int waiting_time;
  int turnaround_time;
} process;

typedef struct Node {
  process *p;
  struct Node *next;
} node;

typedef struct Queue {
  int id;                     //A
  int priority;               //B
  int time_quantum;           //C

  node *front;
  node *rear;
} queue;

//function prototypes
void initialize_queues();
node* create_node(process* p1);
void enqueue(queue* q, process* p1);
process* dequeue(queue* q);
int is_queue_empty(queue* q);
void execute_priority_boost();
int are_processes_done();
void enqueue_to_topmost_queue(int current_time);
void enqueue_to_lower(process* p1);

//global variables
int number_of_processes;
process p[100];
int number_of_queues;
queue q[5];
int priority_boost_time;
queue io;

int main(void) {

  //TODO: read text file and initialize processes and queues

  initialize_queues();

  //TODO: sort queues according to priority (in descending order)

  int current_time = 0;
  process* p1;   //process running in CPU
  process* p2;   //process running in IO

  while (!are_processes_done()) {
    
    //Place incoming processes to highest priority queue (Rule 3)
    enqueue_to_topmost_queue(current_time);

    //Get process with the highest priority (Rule 1)
    if (p1 == NULL || p1->time_quantum_left == 0) {
      //Place process to 1 queue lower
      if (p1)
        enqueue_to_lower(p1);
      
      int i;
      for(i = 0; i < number_of_queues; i++) 
        if (!is_queue_empty(&q[i])) {
          p1 = dequeue(&q[i]);
          break;
        }
    }


    //increment current time
    current_time++;

    //trigger priority boost (Rule 5)
    if (current_time % priority_boost_time == 0)
      execute_priority_boost();

  }
  
  return 0;
}

void readTextFile() {
  FILE *fp;
  int lineCounter;
  char filepath[261] = "./";
  char filename[261];

  float X, Y, S;
  float A[100], B[100], C[100];
  float F[100], G[100], H[100], I[100], J[100];

  printf("Enter filename (include .txt): ");
  scanf("%s", filename);

  strcat(filepath, filename);

  fp = fopen (filepath, "r");

  if (fp != NULL) {
    // checking for non-numeric
    char line[256];
    char *token;

    while (fgets(line, 256, fp)) {
      token = strtok(line, " ");

      while(token != NULL) {
        if (!isStringDigitsOnly(token)) {
          printf("Error: Text file contains non-numeric data and/or invalid values.");
          exit(1);
        }
        token = strtok(NULL, " ");
      } 
    }

    fseek(fp, 0, SEEK_SET);

    if (fscanf(fp, " %f %f %f", &X, &Y, &S) == 3) {
      lineCounter = 0;
      while (lineCounter != X && fscanf(fp, " %f %f %f", &A[lineCounter], &B[lineCounter], &C[lineCounter]) == 3) {
        lineCounter++;
      }

      lineCounter = 0;
      while (lineCounter != Y && fscanf(fp, " %f %f %f %f %f", &F[lineCounter], &G[lineCounter], &H[lineCounter], &I[lineCounter], &J[lineCounter]) == 5) {
        lineCounter++;
      }
    } else {
      printf("Error: Problem reading first line of text file. Make sure it contains 3 integers.");
      exit(1);
    }
  } else {
    printf("\"%s\" not found.", filename);
    exit(1);
  }
  fclose(fp);

  //TODO: CHECK INPUTS
}

void enqueue_to_lower(process* p1) {
  if (p1->queue_index == number_of_queues - 1) {
    enqueue(&q[p1->queue_index], p1);
  }
  else {
    p1->queue_index++;
    enqueue(&q[p1->queue_index], p1);
  }
}

/*
  Move all processes to topmost queue 
  starting from the "queue below the topmost queue"
  to the lowest queue (with respect to priority)
*/
void execute_priority_boost() {
  int i;
  for (i = 1; i < number_of_queues; i++) {
    while (!is_queue_empty(&q[i])) {
      process* p1 = dequeue(&q[i]);
      p1->queue_index = 0;
      enqueue(&q[0], p1);
    }
  }
}

void enqueue_to_topmost_queue(int current_time) {
  int i;
  for (i = 0; i < number_of_processes; i++)
    if (p[i].arrival_time == current_time) {
      p[i].queue_index = 0;
      enqueue(&q[0], &p[i]);
    }
}

int are_processes_done() {
  int flag = 1; //TRUE (processes are done)
  int i;
  for (i = 0; i < number_of_processes; i++) {
    if (p[i].execution_time_left != 0) {
      flag = 0; //FALSE (processes are not done)
      break;
    }
  }
  return flag;
}


/*Queue operations*/

void initialize_queues() {
  int i;
  for (i = 0; i < number_of_queues; i++) {
    q[i].front = NULL;
    q[i].rear = NULL;
  }
}

node* create_node(process *p1) {
  node* qNode = (node*) malloc(sizeof(node));
  qNode->p = p1;
  qNode->next = NULL;
  return qNode;
}

void enqueue(queue* q, process *p1) {
  node* qNode = create_node(p1);

  if (is_queue_empty(q)) {
    q->front = qNode;
    q->rear = qNode;
  }
  else {
    q->rear->next = qNode;
    q->rear = qNode;
  }
}

process* dequeue(queue* q) {
  
  node* qNode = q->front;
  q->front = q->front->next;

  if (q->front == NULL)
    q->rear = NULL;

  process* p = qNode->p;
  
  free(qNode);

  return p;
}

int is_queue_empty(queue* q) {
  return q->rear == NULL;
}
