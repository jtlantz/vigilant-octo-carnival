#include<stdio.h>
#include<stdlib.h>
#include<time.h>

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
}

void multiply_base()
{
    // int a,b,b2,c,i;
	// for(a=0; a<SIZEX*SIZEY;a++)
	// {
	// 	for(b=0;b<SIZEX;b++)
	// 	{
	// 		b2=(b+(a*SIZEX))%(SIZEX*SIZEY);
	// 		c=(a/SIZEX) + a;
	// 		huge_matrixC[c] +=  huge_matrixA[a] * huge_matrixB[b2];

	// 		printf("%ld, %ld, %ld :: ", a,b2,c);
	// 	}
	// 	printf("\n");
	// }
	int i,j,k,a,b,c;
	for(i=0;i<SIZEX;i++)
	{
		for(j=0;j<SIZEY;j++)
		{
			for(k=0;k<SIZEX;k++)
			{
				a=i*SIZEX+j;
				b=(j*SIZEX+k)%(SIZEX*SIZEY);
				c=(i*SIZEX)+k;
				huge_matrixC[c] +=  huge_matrixA[a] * huge_matrixB[b];
				// printf("%ld, %ld, %ld :: ", a,b,c);
			}
		}
	}
	for(i=0;i<SIZEX*SIZEY;i++){
		if(i%SIZEX==0&&i!=0) fprintf(fout, "\n");
		fprintf(fout, "%ld ", huge_matrixC[i]);
	}
	fprintf(fout, "\n");
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
	// Your code here
	//
	// Basically, make sure the result is written on fout
	// Each line represent value in the X-dimension of your matrix
}

void load_matrix()
{
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
}



void multiply()
{
	// Your code here
}

int main()
{
	
	clock_t s,t;
	double total_in_base = 0.0;
	double total_in_your = 0.0;
	double total_mul_base = 0.0;
	double total_mul_your = 0.0;
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
	

	flush_all_caches();

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
	free_all();

	flush_all_caches();

	s = clock();
	load_matrix();
	t = clock();
	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);

	s = clock();
	multiply();
	t = clock();
	total_mul_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the multiply = %f seconds\n", total_mul_your);
	write_results();
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	fclose(ftest);
	free_all();
	compare_results();

	return 0;
}
