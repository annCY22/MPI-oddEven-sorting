#include <stdio.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include<math.h>
int Compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[]){

	int nump,rank;
	int n,localn;
	int *data,*recdata,*recdata2;
	int *temp;
	int d;
	int root_process;
	int sendcounts;
	MPI_Status status;
	int i,j,k;
	double startTime,endTime;
	MPI_Init(&argc, &argv);
    root_process = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nump);
	n =atoi(argv[1]);
 	localn=n/nump;
	recdata =(int*)malloc(sizeof(int)*localn);
	recdata2=(int*)malloc(sizeof(int)*localn);
	temp =(int*)malloc(sizeof(int)*localn);
      if(rank == root_process) {


    	data=(int*)malloc(sizeof(int)*n);
         for(i = 0; i < n; i++) {
            data[i] = rand()%129;
         }
         printf("\nGenerated array :\n");
         for(i=0;i<n;i++){
         	printf("%d ",data[i] );
         }
         printf("\n\n");

    }
    MPI_Scatter(data, localn, MPI_INT, recdata,localn, MPI_INT, 0, MPI_COMM_WORLD)  ; /*Distributing data to other processors */
    startTime = MPI_Wtime();


    qsort(recdata,localn,sizeof(int),Compare);

 /* ##################################### Hypercube comparison ###########################################3 */
    MPI_Barrier(MPI_COMM_WORLD);
    int partner;


	d=(int)(log((double)nump)/log(2));

	int mul = nump/2;
	for(j=d-1; j >= 0; j--){
		partner = rank ^ mul;  // partner = rank ^ (int)(pow(2,(double)i));

		MPI_Sendrecv(recdata,localn,MPI_INT,partner,j,recdata2,localn,MPI_INT,partner,j,MPI_COMM_WORLD,&status);
		for( i =0; i < localn; i++)
			temp[i] = recdata[i];

		if((rank >> j) & 1)
        		    CompareSplitHi(localn,recdata,recdata2,temp);

	        else

            		    CompareSplitLow(localn,recdata,recdata2,temp);

		mul = mul/2;

	}

	

 /* ##################################Odd Even Sorting #################################################### */

    //begin the odd-even sort
    int oddrank,evenrank;

    if(rank%2==0){
    	oddrank=rank-1;
    	evenrank = rank+1;
	}
 	else {
 		oddrank = rank+1;
 		evenrank = rank-1;
	}
/* Set the ranks of the processors at the end of the linear */
if (oddrank == -1 || oddrank == nump)
 oddrank = MPI_PROC_NULL;
if (evenrank == -1 || evenrank == nump)
 evenrank = MPI_PROC_NULL;
MPI_Barrier(MPI_COMM_WORLD);

/*##########################################################################################################*/
int phase = 1;
int count;
int totalcount;
int *temp1=(int*)malloc(localn*sizeof(int));
int *temp2=(int*)malloc(localn*sizeof(int));

int done = 0;
int finaldone ;
int finish=0;
do{

	count = 0;
  	totalcount = 0;

	
	for(i = 0; i <localn; i++){
		temp1[i] = recdata[i];
	
	}



 	if (phase % 2 == 1){		 /* Odd phase */

		 MPI_Sendrecv(recdata, localn, MPI_INT, oddrank, 1, recdata2,localn, MPI_INT, oddrank, 1, MPI_COMM_WORLD, &status);

	}else{			 /* Even phase */

	 	MPI_Sendrecv(recdata, localn, MPI_INT, evenrank, 1, recdata2,localn, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &status);
	}

	phase=phase+1;

	 //extract localn after sorting the two

 	for(i=0;i<localn;i++){
 		temp[i]=recdata[i];
 		}

 	if(status.MPI_SOURCE!=MPI_PROC_NULL) /* To stop processors at the end from participating in sorting when phase =odd */

	{
    		if(rank<status.MPI_SOURCE)
        		CompareSplitLow(localn,recdata,recdata2,temp);
    		else

        		CompareSplitHi(localn,recdata,recdata2,temp);
	}

 	for(i = 0; i < localn; i++){
		if(temp1[i]==recdata[i])
			count=count +1;
	}


 	if (count == localn){

		done = 1;
		finaldone = 1;
		totalcount=totalcount+1;
	}
	else{
		finaldone = 0;
	}




	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&finaldone,1,MPI_INT,rank,MPI_COMM_WORLD); /*Broadcasting to other processors*/
 	for(i = 0; i<nump;i++){
		if(i != rank){
			MPI_Bcast(&done,1,MPI_INT,i,MPI_COMM_WORLD); /*Recieving from other processors*/
			if(done == 1)
				totalcount=totalcount+1;
		}
	}



	if (totalcount == nump)
		finish = 1;
	else

	 	finish =0;


}while(finish == 0);


MPI_Barrier(MPI_COMM_WORLD);


MPI_Gather(recdata,localn,MPI_INT,data,localn,MPI_INT,0,MPI_COMM_WORLD);

endTime = MPI_Wtime();
if(rank==root_process){
FILE *ptr;
ptr = fopen("result.txt","wt");


printf("Final sorted data:");
         for(i=0;i<n;i++){
         	printf("%d ",data[i] );
		fprintf(ptr,"%d  ",data[i] );
         }
    printf("\n");

printf("\nFinished in %f sec\n\n",endTime - startTime);
}
free(data);free(recdata);free(recdata2);free(temp);
MPI_Finalize();

}

/*##########################CompareSplitLow function###################################################*/

CompareSplitLow(int localn, int *recdata,int *recdata2, int *temp){

int i,j,k ;

	for(i=j=k=0;k<localn;k++){
 		if(j==localn||(i<localn && temp[i]<recdata2[j]))
 			recdata[k]=temp[i++];
 		else
 			recdata[k]=recdata2[j++];
 	}

	}
/*################################CompareSplitHi function###########################################*/

CompareSplitHi(int localn, int *recdata,int *recdata2, int *temp){

int i , j ,k ;

	for(i=j=k=localn-1;k>=0;k--){
 		if(j==-1||(i>=0 && temp[i]>=recdata2[j]))
 			recdata[k]=temp[i--];
 		else
 			recdata[k]=recdata2[j--];
 	}
}
