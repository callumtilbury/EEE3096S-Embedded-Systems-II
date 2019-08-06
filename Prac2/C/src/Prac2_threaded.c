#include "Prac2_threaded.h"

float result [SAMPLE_COUNT];
FILE *fptr;
FILE *fptr_acc;

float epsilon = 0.000001;

// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
    int ID = *((int*)Parameter);
    //tic();
    int y;
    //Divide up array into number of threads
    for(y = ID*(SAMPLE_COUNT/Thread_Count); y < (ID+1)*(SAMPLE_COUNT/Thread_Count); y++){
        result[y]=0;
        result[y] += carrier[y]*data[y];
    }
    return 0;
}


// Point of entry into program
int main(int argc, char** argv){
    fptr = fopen("../log.csv","a+");
    fptr_acc = fopen("../GoldenMeasureData.csv","r+");    

    fprintf(fptr, "%d,",Thread_Count);
    
    int j;
    // Initialise everything that requires initialisation
    tic();
    // Spawn threads...
    int       Thread_ID[Thread_Count]; // Structure to keep the thread ID
    pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin

    for(j = 0; j < Thread_Count; j++){ //spawn threads
        Thread_ID[j] = j;
        pthread_create(Thread+j, 0, Thread_Main, Thread_ID+j);
    }

    // Printing stuff is a critical section...
    pthread_mutex_lock(&Mutex);
    printf("Threads created :-)\n");
    pthread_mutex_unlock(&Mutex);

    tic();
    // Wait for threads to finish
    for(j = 0; j < Thread_Count; j++){
        if(pthread_join(Thread[j], 0)){
            pthread_mutex_lock(&Mutex);
            printf("Problem joining thread %d\n", j);
            pthread_mutex_unlock(&Mutex);
        }
    }

  // No more active threads, so no more critical sections required

  printf("All threads have quit\n");
  double t = toc();
  fprintf(fptr, "%lg,", t/1e-3);
  printf("Time taken for threads to run = %lg ms\n", t/1e-3);
  
  // Accuracy check:
    long diff = 0;
    float correct_result = 0;
    for (int j = 0; j < SAMPLE_COUNT; j++) {
        fscanf(fptr_acc, "%f,", &correct_result);
        if(abs(correct_result-result[j]) > epsilon) {
             diff += 1;
        }
    }

    fprintf(fptr, "%f,%d\n", epsilon, diff);
    printf("Errors found with epsilon = %f: %d\n",epsilon,diff);

  fclose(fptr);
  fclose(fptr_acc);

return 0;
}
//------------------------------------------------------------------------------

