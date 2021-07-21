#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

#include "mm.h"



// Task 1: Flush the cache so that we can do our measurement :)
void flush_all_caches()
{
	//For now will just do this with a fixed block size
	//Will work out later how to dynamically figure out cache size
	char* temp;
	//assuming "generouslly" a cache size of 16MB
	int tempSize = 16*1024*1024; //16M * 1024KB / MB * 1024 Bytes / KB
	temp = malloc(tempSize*sizeof(char));
	for(int i=0; i<tempSize;i++){
		//WEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
		temp[i] = 0;
	}
	free(temp);
}

void load_matrix_base()
{
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}

void free_all()
{
	printf("Attempting to free all matrix allocations\n");
	free(huge_matrixA);
	free(huge_matrixB);
	free(huge_matrixC);
	printf("Freeing matricies successful!\n");
}

void multiply_base()
{
	int a,b,c;
	for(a = 0; a < SIZEX; a++){
		for(b=0; b < SIZEY; b++){
			for(c=0; c < SIZEX; c++) {
				huge_matrixC[find_loc(a,b)] += (
					huge_matrixA[find_loc(a,c)] * huge_matrixB[find_loc(c,b)]
				);
			}
		}
	}
}

void compare_results()
{
	long i;
	long temp1, temp2;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fout, "%ld", &temp1);
		fscanf(ftest, "%ld", &temp2);
		printf("temp1 = %ld\ntemp2 = %ld\n", temp1, temp2);
		if(temp1!=temp2)
		{
			printf("Wrong solution!\n");
			exit(1);
		}
	}
}

void write_results()
{
	int i, j;
	for(i = 0; i<SIZEX;i++) 
	{
		for(j=0; j<SIZEY;j++){
			fprintf(fout, "%ld ", huge_matrixC[find_loc(i,j)]);
		}
		fprintf(fout, "\n");
	}
}

void load_matrix()
{
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	

	pthread_t th_load_matrix[3];
	rewind(fin1);
	rewind(fin2);
	
	pthread_create(&th_load_matrix[0], NULL, &load_matrixA, NULL);
	pthread_create(&th_load_matrix[1], NULL, &load_matrixB, NULL);
	pthread_create(&th_load_matrix[2], NULL, &load_matrixC, NULL);

	pthread_join(th_load_matrix[0], NULL);
	pthread_join(th_load_matrix[1], NULL);
	pthread_join(th_load_matrix[2], NULL);
}

void load_matrixA()
{	
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	long i;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
	}
	pthread_exit(0);
}

void load_matrixB()
{	
	long i;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
	}
	pthread_exit(0);
}

void load_matrixC()
{	
	long i;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		huge_matrixC[i] = 0;		
	}
	pthread_exit(0);
}

int find_loc(int row, int col) 
{
	return row*SIZEY + col;
}

void multiply(Task task)
{
	int row, col, blockRow, blockCol, dot;
	row = task.a;
	col = task.b;

	for(blockRow = row; blockRow < row+BLOCKSIZE; blockRow++){
		for(blockCol = col; blockCol < col+BLOCKSIZE; blockCol++){
			for(dot = 0; dot < SIZEX; dot++){
				//we can gaurentee that no other thread will occupy the same (blockRow, blockCol) at the same time so no need to lock this variable;
				huge_matrixC[find_loc(blockRow,blockCol)] += (
					huge_matrixA[find_loc(blockRow,dot)] * huge_matrixB[find_loc(dot,blockCol)]
				);
			}
		}
	}
}	

void baseline()
{
	s = clock();
	load_matrix_base();

	t = clock();
	total_in_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the load = %f seconds\n", total_in_base);

	s = clock();
	multiply_base();
	t = clock();
	total_mul_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the multiply = %f seconds\n", total_mul_base);
}

void* startThread(void* args)
{
	while(1) {
		usleep(10000);
		Task task;
		pthread_mutex_lock(&mutexQueue);
		if(taskCount>0){
			task = task_pool[0];
			int i;
			for(i = 0 ; i<taskCount - 1; i++){
				task_pool[i] = task_pool[i+1];
			}
			taskCount--;
		}
		pthread_mutex_unlock(&mutexQueue);
		multiply(task);
		if(taskCount==0) return;
	}
}

void work_assignment()
{
	int rows, cols;
	for(rows = 0; rows < SIZEY; rows+=BLOCKSIZE){
		for(cols = 0; cols < SIZEX; cols+=BLOCKSIZE){
			task_pool[taskCount].a = rows;
			task_pool[taskCount].b = cols;
			taskCount++;
		}
	}
	return;
}

void improved_matrix_multiply()
{
	flush_all_caches();

	s = clock();
	load_matrix();
	t = clock();

	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);

	
	int blockSize = BLOCKSIZE;
	printf("Trying to multiply using blockSize of %d\n", blockSize);
	int thread_count = 16; //create a thread pool with this many threads
	taskCount = 0; //Set our current task count to 0

	task_pool = malloc((SIZEX*SIZEY/blockSize)*sizeof(Task));
	pthread_t th_pool[thread_count];
	pthread_mutex_init(&mutexQueue, NULL);

	
	int i;
	for(i=0; i<thread_count; i++){
		if(pthread_create(&th_pool[i], NULL, &startThread, NULL) != 0) {
			perror("Failed to create Thread");
		}
	}

	struct timespec t1, t2;
	double elapsedTime;

	clock_gettime(CLOCK_MONOTONIC, &t1);
	
	printf("Trying to submit tasks\n");
	work_assignment();

	printf("work assigned, trying to loop threads now\n");

	for(i=0; i<thread_count; i++){
		if(pthread_join(th_pool[i], NULL) != 0) perror("Failed to create Thread");
	}
	clock_gettime(CLOCK_MONOTONIC, &t2);

	pthread_mutex_destroy(&mutexQueue);
	elapsedTime = (t2.tv_sec - t1.tv_sec);
	elapsedTime += (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

	printf("Total time taken during the multiply = %.3f seconds\n", elapsedTime);
}

int main()
{
	
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");

	// baseline();
	// free_all();
	improved_matrix_multiply();

	printf("Writing results\n");
	write_results();
	printf("Wrote results\n");

	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	fclose(ftest);
	free_all();
	compare_results();

	return 0;
}
