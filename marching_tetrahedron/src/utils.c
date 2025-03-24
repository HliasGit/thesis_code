#include "utils.h"

/**
 * @brief Reads a file and initializes the tensor data (linearized) structure along with its dimensions and origin.
 *
 * @param file_name The path to the file to be read.
 * @param dim Pointer to a Dimensions structure where the dimensions of the tensor will be stored.
 * @param tensor Pointer to a pointer of type dim_t where the tensor data will be allocated and stored.
 * @param origin Pointer to a double array where the origin coordinates will be stored.
 */
void read_file(const char* file_name, Dimensions *dim, dim_t **tensor, double *origin){

    FILE *fptr;
    fptr = fopen(file_name, "rb"); 

    if(fptr == NULL) {
        fprintf(stderr, "Not able to open the file\n");
        exit(-1);
    }

    double dx;
    double dy;
    double dz;
    
    fread(&(dx), sizeof(double), 1, fptr);
    fread(&(dy), sizeof(double), 1, fptr);
    fread(&(dz), sizeof(double), 1, fptr);
    fread(&(origin[0]), sizeof(double), 1, fptr);
    fread(&(origin[1]), sizeof(double), 1, fptr);
    fread(&(origin[2]), sizeof(double), 1, fptr);
    fread(&(dim->x_dim), sizeof(size_t), 1, fptr);
    fread(&(dim->y_dim), sizeof(size_t), 1, fptr);
    fread(&(dim->z_dim), sizeof(size_t), 1, fptr);

    verbose_print("x_dim: %d\n", dim->x_dim);
    verbose_print("y_dim: %d\n", dim->y_dim);
    verbose_print("z_dim: %d\n", dim->z_dim);

    printf("Origin:\n");
    printf("    x: %f\n", origin[0]);
    printf("    y: %f\n", origin[1]);
    printf("    z: %f\n", origin[2]);

    *tensor = (dim_t*)malloc(sizeof(dim_t)*dim->x_dim*dim->y_dim*dim->z_dim);
    
    for (int i=0; i<dim->x_dim; i++){
        for (int j=0; j<dim->y_dim; j++){
            for (int k=0; k<dim->z_dim; k++){
                fread(&(*tensor)[k + j*dim->y_dim + i*dim->y_dim*dim->z_dim], sizeof(dim_t), 1, fptr);
                // verbose_print("%f\n", (*tensor)[i + dim->x_dim*j + k + dim->x_dim*dim->y_dim]);
            }
        }
    }
    fclose(fptr);
}

/**
 * @brief Print the grid values
 * 
 * @param dim Pointer to the dimensions structure
 * @param grid Pointer to the grid values
 */
void print_grid(const Dimensions *dim, const dim_t *grid){
    for (int k=0; k<dim->z_dim; k++){
        for (int j=0; j<dim->y_dim; j++){
            for (int i=0; i<dim->x_dim; i++){
                verbose_print("%f\n", grid[i + dim->x_dim*j + k*dim->x_dim*dim->y_dim]);
            }
        }
    }
}

/**
 * @brief Print the stack, with the coordinates and the value
 * 
 * @param start Pointer to pointer to the beginning of the stack
 */
void print_stack(StackNode *start){
    verbose_print("    Stack content:\n");
    while(start != NULL){
        verbose_print("        Coord: (%d, %d, %d); Val: %f\n",    start->coordinate.x,
                                                            start->coordinate.y,
                                                            start->coordinate.z,
                                                            start->owned_value);
        start = start->next;
    }
}

/**
 * @brief Print to a file the triangle
 * 
 * @param triangle Pointer to the triangle
 * @param count Pointer to the triangle number
 * @param origin Pointer to the origin of the reference system
 */
void print_to_file(Triangle *triangle, int *count, double *origin){
    FILE *fptr;

    // ATOM      1 0    PSE A   0      60.000  58.000  46.500  1.00  1.00           C  
    char str[500];
    snprintf(str, sizeof(str), "ATOM  %5d 0    PSE A   0      %6.3f  %6.3f  %6.3f  1.00  1.00           C\n", 
            (*count)*3+0, (triangle->v1->x), (triangle->v1->y), (triangle->v1->z));
    snprintf(str + strlen(str), sizeof(str) - strlen(str), "ATOM  %5d 0    PSE A   0      %6.3f  %6.3f  %6.3f  1.00  1.00           C\n", 
            (*count)*3+1, (triangle->v2->x), (triangle->v2->y), (triangle->v2->z));
    snprintf(str + strlen(str), sizeof(str) - strlen(str), "ATOM  %5d 0    PSE A   0      %6.3f  %6.3f  %6.3f  1.00  1.00           C\n", 
            (*count)*3+2, (triangle->v3->x), (triangle->v3->y), (triangle->v3->z));

    if ((*count) == 1) {
        fptr = fopen("write.pdb", "w");
        // printf("QUA\n");
        // exit(1);
    } else {
        fptr = fopen("write.pdb", "a");
    }
    fprintf(fptr, "%s", str);

    
    fclose(fptr);
}

/**
 * @brief Print to a file the triangle connections
 * 
 * @param triangle Pointer to the triangle
 * @param count Pointer to the triangle number
 */
void print_connections(Triangle *triangle, int*count){
    FILE *fptr;

    // ATOM      1 0    PSE A   0      60.000  58.000  46.500  1.00  1.00           C  
    char str[500];
    if ((*count)*3+0 > (*count)*3+1) {
        snprintf(str, sizeof(str), "CONECT%5d%5d\n", 
                (*count)*3+1, (*count)*3+0);
    } else {
        snprintf(str, sizeof(str), "CONECT%5d%5d\n", 
                (*count)*3+0, (*count)*3+1);
    }

    if ((*count)*3+1 > (*count)*3+2) {
        snprintf(str + strlen(str), sizeof(str) - strlen(str), "CONECT%5d%5d\n", 
                (*count)*3+2, (*count)*3+1);
    } else {
        snprintf(str + strlen(str), sizeof(str) - strlen(str), "CONECT%5d%5d\n", 
                (*count)*3+1, (*count)*3+2);
    }

    if ((*count)*3+2 > (*count)*3+0) {
        snprintf(str + strlen(str), sizeof(str) - strlen(str), "CONECT%5d%5d\n", 
                (*count)*3+0, (*count)*3+2);
    } else {
        snprintf(str + strlen(str), sizeof(str) - strlen(str), "CONECT%5d%5d\n", 
                (*count)*3+2, (*count)*3+0);
    }

    if ((*count) == 1) {
        fptr = fopen("conn.pdb", "w");
    } else {
        fptr = fopen("conn.pdb", "a");
    }
    fprintf(fptr, "%s", str);
    fclose(fptr);
}

/**
 * @brief Merges two files into one
 * 
 * @param atoms Pointer to the atoms file
 * @param conn Pointer to the connections file 
 */
void merge_files(char *atoms, char* conn){
    FILE *f_atoms = fopen(atoms, "a");
    FILE *f_conn = fopen(conn, "r");

    if (f_atoms == NULL || f_conn == NULL) {
        fprintf(stderr, "Error opening files for merging\n");
        exit(-1);
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), f_conn) != NULL) {
        fputs(buffer, f_atoms);
    }

    fclose(f_atoms);
    fclose(f_conn);
}