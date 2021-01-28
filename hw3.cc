#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include <omp.h>

// 1. Read Input file (refer to hw1 MPI_Read_at())
// 2. All-Pair-Shortest-Path Algorithm
// 3. Output file. (refer to hw1 MPI_Write_at())

#define NUM_THREADS 20
#define CHUNKSIZE 5000

#define INF 1073741823 // Infinite far, no available path.

// #define PRINT_ADJ
#define APSP
// #define PRINT_ANS
#define WRITE_OUTPUT_FILE

// #define WTIME


int main(int argc, char** argv) {

    #ifdef WTIME
    double start; 
    double end; 
    start = omp_get_wtime(); 
    double Input_starttime; 
    double Input_endtime; 
    double Input_TotalTime = 0.0;
    #endif


    // =============== Read INPUT_FILE ============ //

    int V_and_E[2]; //buffer to store num_of_v and num_of_edges
    int num_of_vertices;
    int num_of_edges;

    int print_boundary;

    // store input data
    int* input_buf;

    // store adj_matrix
    int* output_buf;

    // int** adj_matrix;
    #ifdef WTIME
    Input_starttime = omp_get_wtime();
    #endif
    FILE *pFileRead;
    pFileRead = fopen( argv[1] ,"r");
    if( NULL == pFileRead ){
        printf( "open failure" );
        return 1;
    }
    else{
        // first read V and E.
        fread(V_and_E, sizeof(int), 2, pFileRead);
        #ifdef WTIME
        Input_TotalTime += omp_get_wtime() - Input_starttime;
        #endif
        num_of_vertices = V_and_E[0];
        num_of_edges = V_and_E[1];
        
        output_buf = (int*)malloc(sizeof(int)*num_of_vertices*num_of_vertices);

        // ============== Intialize adjacency matrix to be INF ================== //
        // #pragma omp parallel for schedule(static) collapse(2)
        #pragma omp parallel for schedule(static)
        for(int i=0; i< num_of_vertices; i++){
            #pragma omp parallel for schedule(static)
            for(int j=0; j< num_of_vertices; j++){
                if(i==j) output_buf[i*num_of_vertices+j] = 0;
                else output_buf[i*num_of_vertices + j] = INF;
            }
        }       


        #ifdef PRINT_ADJ
        print_boundary = 10;
        for(int i=0; i< print_boundary; i++){
            printf("%d      ", i);
            for(int j=0; j< print_boundary; j++){
                if(j==print_boundary-1) printf(" %d\n",adj_matrix[i][j]);
                else printf(" %d",adj_matrix[i][j]);
                // if(j==print_boundary-1) std::cout<<adj_matrix[i][j]<<std::endl;
                // else std::cout<<adj_matrix[i][j];

            }
        }
        #endif

        input_buf = (int*)malloc(sizeof(int)* (3*num_of_edges));
        // read input file into buffer "input_buf"
        // ======== NOTE !!! You haven't close the file, so start from the first edge! Don't need to offset! ===== //
        // fread(input_buf, sizeof(int), BUFFER_SIZE, pFileRead );
        #ifdef WTIME
        Input_starttime = omp_get_wtime();
        #endif
        fread(input_buf, sizeof(int), 3*(num_of_edges), pFileRead);
        #ifdef WTIME
        Input_TotalTime += omp_get_wtime() - Input_starttime;
        #endif

        #pragma omp parallel for schedule(static)
        for(int i=0; i < num_of_edges; i++){
            output_buf[(input_buf[i*3])*num_of_vertices + input_buf[i*3 +1] ] = input_buf[i*3 +2]; 
            // printf("Edge: %d src: %d dst: %d => weight: %d\n", i, input_buf[i*3], input_buf[i*3 +1], adj_matrix[input_buf[i*3]][input_buf[i*3 +1]]);
        }
    }

    fclose(pFileRead);

    
    // ============== print out adjacency matrix ================== //
    #ifdef PRINT_ADJ
        print_boundary = 7; // for example.
        printf("\n\nAdjacency Matrix: \n");

        printf("    dst:");
        for(int k=0; k<print_boundary-1; k++){
            printf(" %d", k);
        }
        printf(" %d\n", print_boundary-1);
        printf("src: \n");

        for(int i=0; i< print_boundary; i++){
            printf("%d      ", i);
            for(int j=0; j< print_boundary; j++){
                if(j==print_boundary-1) printf(" %d\n",adj_matrix[i][j]);
                else printf(" %d",adj_matrix[i][j]);
            }
        }
    #endif

    // num_of_vertices V
    // num_of_edges    E
    // adj_matrix      adjacency matrix 

    // ============== All Pair Shortest Path ================== //
    // double Computation_starttime;
    // Computation_starttime = omp_get_wtime();

    #ifdef APSP  

    for( int k=0; k<num_of_vertices; k++){
        // #pragma omp parallel for schedule(static) collapse(2)
        #pragma omp parallel for schedule(static) 
        for(int i=0; i<num_of_vertices; i++){
            #pragma omp parallel for schedule(static) 
            for(int j=0; j<num_of_vertices; j++){
                // if(i==j) continue;
                if( output_buf[i*num_of_vertices+j] > output_buf[i*num_of_vertices + k] + output_buf[k*num_of_vertices + j]){
                    output_buf[i*num_of_vertices+j] = output_buf[i*num_of_vertices + k] + output_buf[k*num_of_vertices + j];
                }
                // else do nothing.
            }
        }
    }
    #endif



    #ifdef PRINT_ANS
        char FILE_NAME[20] = "cases/c10.1.out";
        int* ans_buf = (int*)malloc(sizeof(int)*num_of_vertices*num_of_vertices);
        FILE *pFileAns;
        pFileAns = fopen( FILE_NAME ,"r");
        if( NULL == pFileAns ){
            printf( "open failure" );
            return 1;
        }
        else{
            // first read V and E.
            fread(ans_buf, sizeof(int), num_of_vertices*num_of_vertices, pFileAns);
        }
        fclose(pFileRead);

        int start_index = num_of_vertices;
        int threshold = 2*num_of_vertices; //first row dst(0,V);
        for(int i= start_index; i< threshold; i++){
            printf("my_ans %d | ans: %d \n", output_buf[i], ans_buf[i]);
        }
    #endif

    #ifdef WTIME
    double Output_starttime;
    Output_starttime = omp_get_wtime();
    #endif

    // TODO 2. create threads to cooperately write file.
    #ifdef WRITE_OUTPUT_FILE
    // write to a test_input.out file.
    FILE *output_pFile;
    output_pFile = fopen(argv[2],"wb");
    if( NULL == output_pFile ){
        printf( "open failure" );
        return 1;
    }else{
        fwrite(output_buf, sizeof(int), num_of_vertices*num_of_vertices, output_pFile);
        // fwrite(output_buf, sizeof(int), 100, output_pFile);
        // fwrite(input_buf,sizeof(int), n ,pFile);
    }
    fclose(output_pFile);
    // printf("Done writing output file.\n");
    #endif

    #ifdef WTIME
    double Output_TotalTime;
    Output_TotalTime = omp_get_wtime()-Output_starttime;
    printf("Input Time %.8f \n", Input_TotalTime);
    printf("Output Time %.8f\n", Output_TotalTime);
    #endif

    #ifdef WTIME
    end = omp_get_wtime(); 
    printf("Computation Time %.8f seconds\n", end - start - Output_TotalTime - Input_TotalTime);
    printf("Total Time %.8f seconds\n", end - start);
    #endif

    return 0;

}

