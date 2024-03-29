#include "Prac2.h"

extern float data [SAMPLE_COUNT];
extern float carrier[SAMPLE_COUNT];

float result [SAMPLE_COUNT];

float epsilon = 0.000001;

FILE *fptr;
FILE *fptr_acc;

int main(int argc, char**argv){
    fptr = fopen("../log.csv","a+");
    fptr_acc = fopen("../GoldenMeasureData.csv","r+");

    printf("Running Unthreaded Test\n");
    printf("Precision sizeof %d\n", sizeof(result[0]));
    fprintf(fptr, "%d,",sizeof(result[0]));

    tic(); // start the timer
    for (int i = 0;i<SAMPLE_COUNT;i++ ){
        result[i] = data[i] * carrier[i];
    }
    double t = toc();
    printf("Time: %lf ms\n",t/1e-3);
    fprintf(fptr, "%lf,",t/1e-3);

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
    printf("End Unthreaded Test\n");

    fclose(fptr);
    fclose(fptr_acc);

    return 0;
}

