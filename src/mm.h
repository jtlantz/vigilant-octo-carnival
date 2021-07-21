#define SIZEX 2000
#define SIZEY 2000
#define BLOCKSIZE 100

typedef struct Task{
    int a,b;
}Task;

long * huge_matrixA;
long * huge_matrixB;
long * huge_matrixC;

FILE *fin1, *fin2, *fout, *ftest;

void flush_all_caches();
void load_matrix_base();
void free_all();
void multiply_base();
void compare_results();

// Your job is to finish these three functions in the way that it is fast
void write_results();
void load_matrix();
void multiply(Task task);

// Any additional variables needed go here

FILE *fin1_better, *fin2_better;
clock_t s,t;
Task *task_pool;
double total_in_base = 0.0;
double total_in_your = 0.0;
double total_mul_base = 0.0;
double total_mul_your = 0.0;
int taskCount;
pthread_mutex_t mutexQueue;

// Any helper functions go here
int find_loc(int x, int y);
void load_matrixA();
void load_matrixB();
void load_matrixC();