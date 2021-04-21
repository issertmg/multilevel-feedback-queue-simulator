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

//global variables
int number_of_processes;
process p[100];
int number_of_queues;
queue q[5];
int priority_boost_time;

int main(void) {

  //TODO: read text file and initialize processes and queues

  initialize_queues();

  //TODO: sort queues according to priority (in descending order)

  int current_time = 0;

  while (!are_processes_done()) {
    
    //Place incoming processes to highest priority queue (Rule 3)
    enqueue_to_topmost_queue(current_time);









    //increment current time
    current_time++;

    //trigger priority boost (Rule 5)
    if (current_time % priority_boost_time == 0)
      execute_priority_boost();

  }
  
  return 0;
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
      enqueue(&q[0], p1);
    }
  }
}

void enqueue_to_topmost_queue(int current_time) {
  int i;
  for (i = 0; i < number_of_processes; i++)
    if (p[i].arrival_time == current_time)
      enqueue(&q[0], &p[i]);
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
