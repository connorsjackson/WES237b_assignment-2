#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

#define CHECK_ERR(err, msg)                           \
    if (err != CL_SUCCESS)                            \
    {                                                 \
        fprintf(stderr, "%s failed: %d\n", msg, err); \
        exit(EXIT_FAILURE);                           \
    }

void NaiveMatrixMultiply(Matrix *input0, Matrix *input1, Matrix *result)
{
    //@@ Insert code to implement naive matrix multiply here
    int row_size_input0 = (*input0).shape[0]; //i iterator, # rows
    int col_size_input1 = input1->shape[1]; //j iterator, # cols
    int col_size_input0 = input0->shape[1]; //k iterator, # cols; could have used row_size_input1 instead.

    int blockSize=1;
    int row_size_input1 = (*input1).shape[1]; //# rows
    int blockSizeMax = 4;
    for (int i = 1; i<blockSizeMax; i++){
        if ((col_size_input0 % i == 0) && (row_size_input1 % i == 0)){
            blockSize = i;
        }
    }
    //Is vector a multiple of 4? If not, how many extra rows?
    int remainder = (col_size_input0) % 4;
    //Where I stop first for loop and start second IF remainder is non-zero
    int Loop_Before_Remainder = (col_size_input0) - remainder;

    for (int i = 0; i<row_size_input0; i+=blockSize){
        for (int j = 0; j<col_size_input1;j+=blockSize){
            for (int blockRow = i; blockRow < i + blockSize; blockRow++){
                for (int blockCol = j; blockCol < j+blockSize; blockCol++){
                    for (int k = 0; k<Loop_Before_Remainder; k+=4){
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+k] * (*input1).data[k*col_size_input1+blockCol];
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+(k+1)] * (*input1).data[(k+1)*col_size_input1+blockCol];
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+(k+2)] * (*input1).data[(k+2)*col_size_input1+blockCol];
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+(k+3)] * (*input1).data[(k+3)*col_size_input1+blockCol];
                    }
                    if (remainder != 0){
                        for (int k = Loop_Before_Remainder; k<col_size_input0; k++){
                            (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+k] * (*input1).data[k*col_size_input1+blockCol];
                        }
                    }
                }
            }
        }
    }




    /* This was attempt 1 that worked but no safeguard for remainder.
    for (int i = 0; i<row_size_input0; i+=blockSize){
        for (int j = 0; j<col_size_input1;j+=blockSize){
            for (int blockRow = i; blockRow < i + blockSize; blockRow++){
                for (int blockCol = j; blockCol < j+blockSize; blockCol++){
                    for (int k = 0; k<col_size_input0; k+=4){
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+k] * (*input1).data[k*col_size_input1+blockCol];
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+(k+1)] * (*input1).data[(k+1)*col_size_input1+blockCol];
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+(k+2)] * (*input1).data[(k+2)*col_size_input1+blockCol];
                        (*result).data[blockRow*col_size_input1+blockCol] += (*input0).data[blockRow*col_size_input0+(k+3)] * (*input1).data[(k+3)*col_size_input1+blockCol];
                    }
                }
            }
        }
    }
    */
    


    /* My pseudocode is below, more mathematical
    for (int i = 0; i<row_size_input0; i++){
        for (int j = 0; j<col_size_input1;j++){
            for (int k = 0; k<col_size_input0; k++){
                result[i,j] += input_0[i,k] * input_1[k,j];
            }
        }
    }
    */
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <input_file_0> <input_file_1> <answer_file> <output_file>\n", argv[0]);
        return -1;
    }

    const char *input_file_a = argv[1];
    const char *input_file_b = argv[2];
    const char *input_file_c = argv[3];
    const char *input_file_d = argv[4];

    // Host input and output vectors and sizes
    Matrix host_a, host_b, host_c, answer;
    
    cl_int err;

    err = LoadMatrix(input_file_a, &host_a);
    CHECK_ERR(err, "LoadMatrix");

    err = LoadMatrix(input_file_b, &host_b);
    CHECK_ERR(err, "LoadMatrix");

    err = LoadMatrix(input_file_c, &answer);
    CHECK_ERR(err, "LoadMatrix");

    int rows, cols;
    //@@ Update these values for the output rows and cols of the output
    //@@ Do not use the results from the answer matrix
    rows = host_a.shape[0];
    cols = host_b.shape[1];

    // Allocate the memory for the target.
    host_c.shape[0] = rows;
    host_c.shape[1] = cols;
    host_c.data = (float *)malloc(sizeof(float) * host_c.shape[0] * host_c.shape[1]);

    // Call your matrix multiply.
    NaiveMatrixMultiply(&host_a, &host_b, &host_c);

    // // Call to print the matrix
    //PrintMatrix(&host_c);

    // Check the result of the matrix multiply
    CheckMatrix(&answer, &host_c);

    // Save the matrix
    SaveMatrix(input_file_d, &host_c);

    // Release host memory
    free(host_a.data);
    free(host_b.data);
    free(host_c.data);
    free(answer.data);

    return 0;
}