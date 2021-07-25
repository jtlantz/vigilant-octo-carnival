#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

#include "gen.h"

int main(int argc, char argv[])
{
    FILE *in1, *in2;
    in1 = fopen("./input1.in","w");
	in2 = fopen("./input2.in","w");
    int i, j;

    for(i = 0; i < 10000;i++){
        for(j = 0; j<10000; j++){
            fprintf(in1, "%ld ", random()%5+1);
            fprintf(in2, "%ld ", random()%5+1);
        }
        fprintf(in1, "\n");
        fprintf(in2, "\n");
    }
    fclose(in1);
    fclose(in2);
}