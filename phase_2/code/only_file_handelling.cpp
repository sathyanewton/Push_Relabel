#include <stdio.h>



/* Global variables */

FILE *fp,*fp2;
int operation[5];
int edgeCount[3];
int nodeCount[3];
int var1[3],var2[3];

int main()
{
	fp = fopen("out.txt", "r");
            if (fp == NULL) {
                    printf("Can't open input file!\n");
                    while(1);
                }
            if(fscanf(fp, "%d %d", &nodeCount, &edgeCount) != EOF)
               {
               printf("\n Node count and Edge count are %d %d\n", *nodeCount, *edgeCount);
               }
               while (fscanf(fp, "%d %d %d", &operation,&var1,&var2) != EOF)
               {      
               //	printf("\nThe values are %d %d %d", *operation, *var1, *var2);
               }
            

	return 0;
}