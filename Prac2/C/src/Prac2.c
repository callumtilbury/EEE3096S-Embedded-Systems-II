#include "Prac2.h"

extern float data [SAMPLE_COUNT];
extern float carrier[SAMPLE_COUNT];

float result [SAMPLE_COUNT];

FILE *fptr;
FILE *fptr_acc;

int main(int argc, char**argv){
    fptr = fopen("../log.csv","a+");
    fptr_acc = fopen("../GoldenMeasureData.csv","r+")

    fprintf(fptr, "C,");
    printf("Running Unthreaded Test\n");
    fprintf(fptr, "Unthreaded,");
    printf("Precision sizeof %d\n", sizeof(float));
    fprintf(fptr, "%d,",sizeof(float));

    float correct_result = 0;
    tic(); // start the timer
    for (int i = 0;i<SAMPLE_COUNT;i++ ){
        result[i] = data[i] * carrier[i];
        fscanf(inFile, "%f,", correct_result);
	printf("%f\n",correct_result-result[i]);
    }
    double t = toc();
    printf("Time: %lf ms\n",t/1e-3);
    fprintf(fptr, "%lf\n",t/1e-3);
    printf("End Unthreaded Test\n");
    return 0;
}

