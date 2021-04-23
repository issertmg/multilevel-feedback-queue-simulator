/*************************************************************** 
Names: GAGAN, Isser Troy M.
       MATIAS, Maria Angela Mikaela E.
Group: 40
Section: S15
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Process {
  /* input*/
  int id;                     //F
  int arrival_time;           //G
  int total_execution_time;   //H
  int io_burst_time;          //I
  int io_frequency;           //J
  
  /* addtnl attributes */
  int execution_time_left;    //time left from total_execution_time
  int io_burst_timer;         //to check if process will withhold cpu already; initialized to 0
  int io_burst_time_left;     //time left from io_burst_time; initialized to io_burst_time
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
process* get_highest_priority_process();
void update_burst_left_in_IO();
void remove_completed_IO(int current_time);
int has_ready_higher_priority_job (process* p1);
void show_processes();
void show_output();
int are_all_queues_empty();
void initialize_processes();
void compute_waiting_turnaround();

int isStringDigitsOnly(const char *str);
void readTextFile();
queue* createQueue();

//global variables
int number_of_processes;
process p[100];
int number_of_queues;
queue q[5];
int priority_boost_time;
queue io;

int main(void) {

  //TODO: read text file and initialize processes and queues
  readTextFile();
  show_processes();

  initialize_processes();
  initialize_queues();

  //TODO: sort queues according to priority (in descending order)

  int current_time = 0;
  process* p1 = NULL;   //process running in CPU
  int priority_boost_flag = 0;
  while (!are_processes_done()) {
    
    //Place incoming processes to highest priority queue (Rule 3)
    enqueue_to_topmost_queue(current_time);

    //trigger priority boost (Rule 5)
    if (current_time % priority_boost_time == 0)
      priority_boost_flag = 1;
    if (priority_boost_flag && p1 == NULL) {
      execute_priority_boost();
      priority_boost_flag = 0;
    }
    
                
    //Get process with the highest priority (Rule 1)
    if (p1 == NULL && !are_all_queues_empty()) {
      p1 = get_highest_priority_process();

      //p1->time_quantum_left = q[p1->queue_index].time_quantum;

      //list start time for CPU burst & queue ID
      char str[10];
      sprintf(str, "%d", q[p1->queue_index].id);
      strcpy(p1->queue_id[p1->start_end_array_size], str);
      p1->start_time[p1->start_end_array_size] = current_time;
    }
    
    //increment current time
    current_time++;

    //decrease burst_time_left for all process in IO 
    update_burst_left_in_IO();
    //Remove processes with complete IO burst from IO (*and update end time)
    remove_completed_IO(current_time);
    
    
    if (p1) {
      p1->execution_time_left--;
      p1->time_quantum_left--;
      p1->io_burst_timer++;

      //Remove from CPU and place to IO
      if (p1->io_burst_timer == p1->io_frequency && p1->io_burst_time != 0 && p1->execution_time_left != 0) {
        //list end time for CPU burst & update start_end_array_size
        p1->end_time[p1->start_end_array_size] = current_time;
        p1->start_end_array_size++;
        
        p1->io_burst_timer = 0;
        p1->io_burst_time_left = p1->io_burst_time;
        enqueue(&io, p1);
      
        //list start time for IO burst
        strcpy(p1->queue_id[p1->start_end_array_size], "IO");
        p1->start_time[p1->start_end_array_size] = current_time;

        p1 = NULL;
      }
      else if (p1->time_quantum_left == 0 && p1->execution_time_left != 0) {
        //List end time for CPU & increase array size
        p1->end_time[p1->start_end_array_size] = current_time;
        p1->start_end_array_size++;
        enqueue_to_lower(p1);
        p1 = NULL;
      }
      else if (p1->execution_time_left == 0) {
        //List end time for CPU & increase array size
        p1->end_time[p1->start_end_array_size] = current_time;
        p1->start_end_array_size++;
        p1 = NULL;
      }

      //TODO: preemptive process
      else if (has_ready_higher_priority_job (p1)) {
        //list end time for CPU burst & update start_end_array_size
        p1->end_time[p1->start_end_array_size] = current_time;
        p1->start_end_array_size++;
        enqueue(&q[p1->queue_index], p1);
        p1 = NULL;
      }
    }

  }
  compute_waiting_turnaround();
  show_output();
  
  return 0;
}

void readTextFile() {
  FILE *fp;
  int queueLineCounter;
  int processLineCounter;
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
    
    /*
    // checking for non-numeric
    char line[256];
    char *token;

    while (fgets(line, 256, fp)) {
      token = strtok(line, " ");
      int tokenCounterPerLine = 0;

      while(token != NULL) {
        if (!isStringDigitsOnly(token)) {
          printf("Error: Text file contains non-numeric data and/or invalid values.");
          exit(1);
        }
        tokenCounterPerLine++;
        token = strtok(NULL, " ");
      } 

      if (tokenCounterPerLine != 3 || tokenCounterPerLine != 5) {
        printf("Error: Each line in the text file should have either 3 or 5 integers.");
        exit(1);
      }
    }

    fseek(fp, 0, SEEK_SET);
    */

    if (fscanf(fp, " %f %f %f", &X, &Y, &S) == 3) {
      queueLineCounter = 0;
      /*TODO: do {
        if (fscanf(fp, " %f %f %f", &A[queueLineCounter], &B[queueLineCounter], &C[queueLineCounter]) == 3)
          queueLineCounter++;
        
        if (queueLineCounter > X)
      } while (queueLineCounter <= X) */;

      //TEMPORARY FOR TESTING ONLY (U CAN DELETE IF U WANT)
      int i;
      for (i = 0; i < X; i++)
        fscanf(fp, " %f %f %f", &A[i], &B[i], &C[i]);
      for (i = 0; i < Y; i++)
        fscanf(fp, " %f %f %f %f %f", &F[i], &G[i], &H[i], &I[i], &J[i]);

    } else {
      printf("Error: Problem reading first line of text file. Make sure it contains 3 integers.");
      exit(1);
    }
  } else {
    printf("\"%s\" not found.", filename);
    exit(1);
  }
  fclose(fp);

  //TODO: check inputs errorCheckInputs (X, Y, S, A, B, C, F, G, H, I, J, queueLineCounter, processLineCounter);

  //TODO: place in global variables
  
    number_of_queues = X;
    number_of_processes = Y;
    priority_boost_time = S;

    for (int i = 0; i < number_of_queues; i++) {
      q[i].id = (int) A[i];
      q[i].priority = (int) B[i];
      q[i].time_quantum = (int) C[i];
    } 

    for (int j = 0; j < number_of_processes; j++) {
      p[j].id = (int) F[j];
      p[j].arrival_time = (int) G[j];
      p[j].total_execution_time = (int) H[j];
      p[j].io_burst_time = (int) I[j];
      p[j].io_frequency = (int) J[j];
    }
}

void errorCheckInputs (float X, float Y, float S, 
                      float A[], float B[], float C[],
                      float F[], float G[], float H[], float I[], float J[],
                      int queueLines, int processLines  
                      ) {
  
  if (X < 2 || X > 5) {
    printf("Error: Specified CPU scheduling algorithm (X) is invalid. Must be a value from 2 to 5.");
    exit(1);
  }
  if (Y < 3 || Y > 100) {
    printf("Error: Specified CPU scheduling algorithm (X) is invalid. Must be a value from 3 to 100.");
    exit(1);
  }

  /* FIXME: NOT SURE!
  if (S < 0 || G < 0 || H < 0 || I < 0 || J < 0) {
    printf("Error: Time variables cannot be negative.");
    exit(1);
  } */
  int i, j;
  if (i == 0 && j != 0 || j == 0 && i != 0) {
    printf("Error: For processes without I/O, the I/O burst time (I) and its frequency (J) must be equal to 0.");
    exit(1);
  }
}

int isStringDigitsOnly(const char *str) {
  while (*str) {
    if (*str != '\n' && *str != ' ' && isdigit(*str++) == 0) 
      return 0;

    *str++;
  }
  return 1;
}

void enqueue_to_lower(process* p1) {
  if (p1->queue_index == number_of_queues - 1) {
    enqueue(&q[p1->queue_index], p1);
  }
  else {
    p1->queue_index++;
    enqueue(&q[p1->queue_index], p1);
  }
  p1->time_quantum_left = q[p1->queue_index].time_quantum;
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
      p1->time_quantum_left = q[0].time_quantum;
      enqueue(&q[0], p1);
    }
  }
}

void enqueue_to_topmost_queue(int current_time) {
  int i;
  for (i = 0; i < number_of_processes; i++)
    if (p[i].arrival_time == current_time) {
      p[i].queue_index = 0;
      p[i].time_quantum_left = q[0].time_quantum;
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
  io.front = NULL;
  io.rear = NULL;
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

process* get_highest_priority_process() {
  process* p1 = NULL;
  int i;
  for(i = 0; i < number_of_queues; i++) 
    if (!is_queue_empty(&q[i])) {
      p1 = dequeue(&q[i]);
      break;
    }
  return p1;
}

void update_burst_left_in_IO() {
  queue* temp = createQueue();
  process* p1;

  while (!is_queue_empty(&io)) {
    p1 = dequeue(&io);
    p1->io_burst_time_left--;
    enqueue(temp, p1);
  }

  while (!is_queue_empty(temp)) {
    p1 = dequeue(temp);
    enqueue(&io, p1);
  }
}

void remove_completed_IO(int current_time) {
  int i;
  queue* temp = createQueue();
  process* p1;

  while (!is_queue_empty(&io)) {
    p1 = dequeue(&io);
    enqueue(temp, p1);
  }

  while (!is_queue_empty(temp)) {
    p1 = dequeue(temp);
    if (p1->io_burst_time_left != 0)
      enqueue(&io, p1);
    else {
      //list end time for IO and update array size
      p1->end_time[p1->start_end_array_size] = current_time;
      p1->start_end_array_size++;

      if (p1->time_quantum_left == 0) 
        enqueue_to_lower(p1);
      else
        enqueue(&q[p1->queue_index], p1);
    }
  }
}

int has_ready_higher_priority_job (process* p1) {
  int flag = 0; //FALSE
  int i;
  for (i = 0; i < p1->queue_index; i++) 
    if (!is_queue_empty(&q[i])) {
      flag = 1; //TRUE
      break;
    }
   
  return flag;
}

void show_processes() {
  int i;
  for (i = 0; i < number_of_processes; i++)
    printf("P[%d] %d %d %d %d \n", p[i].id, p[i].arrival_time, p[i].total_execution_time,
            p[i].io_burst_time, p[i].io_frequency);
}

void show_output() {
  printf("**************************************** \n");
  int i, j;
  for (i = 0; i < number_of_processes; i++) {
    printf("P[%d]\n", p[i].id);
    for (j = 0; j < p[i].start_end_array_size; j++) {
      if (strcmp(p[i].queue_id[j], "IO") == 0)
        printf("[IO] ");
      else
        printf("Q[%s] ", p[i].queue_id[j]);
      printf("Start time: %d \t End time: %d \n", p[i].start_time[j], p[i].end_time[j]);
    }
    printf("Waiting time: %d \n", p[i].waiting_time);
    printf("Turnaround time: %d \n", p[i].turnaround_time);
    printf("**************************************** \n");
  }
}

int are_all_queues_empty() {
  int flag = 1; //TRUE
  int i;
  for (i = 0; i < number_of_queues; i++)
    if (!is_queue_empty(&q[i])) {
      flag = 0;
      break;
    }

  return flag;
}

void initialize_processes() {
  int i;
  for (i = 0; i < number_of_processes; i++) {
    p[i].execution_time_left = p[i].total_execution_time;
    p[i].start_end_array_size = 0;
  }
    
}

queue* createQueue() {
  queue* q = (queue*) malloc(sizeof(queue));
  q->front = NULL;
  q->rear = NULL;
  return q;
}

void compute_waiting_turnaround() {
  int i, j;
  for (i = 0; i < number_of_processes; i++) {
    
    int time_in_IO = 0;
    for (j = 0; j < p[i].start_end_array_size; j++) {
      if (strcmp(p[i].queue_id[j], "IO") == 0)
        time_in_IO += (p[i].end_time[j] - p[i].start_time[j]);
    }

    p[i].turnaround_time = p[i].end_time[p[i].start_end_array_size - 1] - p[i].arrival_time;
    p[i].waiting_time = p[i].turnaround_time - p[i].total_execution_time - time_in_IO;
  }
}